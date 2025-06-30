//==============================================================
//  Minimal, self-contained “MultiPatch” runtime library
//  • Cartesian (1 patch)
//  • Spherical shell (1 patch)
//  • Cubed-sphere composite (7 patches: 1 Cartesian core + 6 wedges)
//  GPU-ready: every callable has __host__ __device__ (“HD”) qualifiers
//==============================================================
#pragma once
#include <array>
#include <cmath>
#include <cstddef>   // std::size_t

//----------------------------------
//  Host / device qualifier helper
//----------------------------------
#ifndef HD
#  define HD __host__ __device__
#endif

//----------------------------------
//  Common math helpers + typedefs
//----------------------------------
using Coord = std::array<double,3>;
template<class T> HD constexpr T sq(T x) { return x * x; }

//==============================================================
//  1. Patch metadata (“meta”) types
//==============================================================
struct CartesianMeta { /* add bounds if you wish */ };

struct SphericalMeta {
  double r_min, r_max;               // inner / outer radius
};

enum class Face { PX, NX, PY, NY, PZ, NZ };

struct CubedSphereMeta {
  Face   face;                       // which cube face
  double r_inner, r_outer;           // radial extent of the wedge
};

//==============================================================
//  2. PatchMap: table of function pointers
//==============================================================
struct PatchMap {
  HD Coord (*local_to_global)(const Coord&, const void*) = nullptr;
  HD Coord (*global_to_local)(const Coord&, const void*) = nullptr;
};

//==============================================================
//  3. Concrete mapping functions
//==============================================================
//-- Cartesian -------------------------------------------------
HD inline Coord cart_local_to_global(const Coord& l, const void*) { return l; }
HD inline Coord cart_global_to_local(const Coord& g, const void*) { return g; }

//-- Spherical shell ------------------------------------------
HD inline Coord sph_local_to_global(const Coord& l, const void* m)
{
  const auto* p = static_cast<const SphericalMeta*>(m);
  const double rho   = l[0];               // [0,1]
  const double theta = l[1];               // [0,2π]
  const double phi   = l[2];               // [0,π]
  const double r = p->r_min + rho * (p->r_max - p->r_min);
  return { r * std::sin(phi) * std::cos(theta),
           r * std::sin(phi) * std::sin(theta),
           r * std::cos(phi) };
}
HD inline Coord sph_global_to_local(const Coord& g, const void* m)
{
  const auto* p = static_cast<const SphericalMeta*>(m);
  const double x = g[0], y = g[1], z = g[2];
  const double r = std::sqrt(sq(x)+sq(y)+sq(z));
  return { (r - p->r_min) / (p->r_max - p->r_min),   // ρ
           std::atan2(y,x),                          // θ
           std::acos(z/r) };                         // φ
}

//-- Cubed-sphere wedge ---------------------------------------
HD inline Coord cube_local_to_global(const Coord& l, const void* m)
{
  const auto* p = static_cast<const CubedSphereMeta*>(m);
  const double xi  = l[0], eta = l[1], rho = l[2];
  const double r   = p->r_inner + rho * (p->r_outer - p->r_inner);
  const double d   = std::sqrt(1.0 + xi*xi + eta*eta);

  switch (p->face) {
    case Face::PX: return {  r/d          ,  r*xi/d        ,  r*eta/d };
    case Face::NX: return { -r/d          , -r*xi/d        ,  r*eta/d };
    case Face::PY: return { -r*xi/d       ,  r/d           ,  r*eta/d };
    case Face::NY: return {  r*xi/d       , -r/d           ,  r*eta/d };
    case Face::PZ: return {  r*xi/d       ,  r*eta/d       ,  r/d     };
    case Face::NZ: return {  r*xi/d       , -r*eta/d       , -r/d     };
  }
  return {0,0,0}; // unreachable
}

HD inline Coord cube_global_to_local(const Coord& g, const void* m)
{
  const auto* p = static_cast<const CubedSphereMeta*>(m);
  const double x=g[0], y=g[1], z=g[2];
  const double r = std::sqrt(sq(x)+sq(y)+sq(z));

  double xi=0, eta=0;
  switch (p->face) {
    case Face::PX: xi =  y/x;  eta =  z/x;  break;
    case Face::NX: xi = -y/x;  eta =  z/x;  break;
    case Face::PY: xi = -x/y;  eta =  z/y;  break;
    case Face::NY: xi =  x/y;  eta =  z/y;  break;
    case Face::PZ: xi =  x/z;  eta =  y/z;  break;
    case Face::NZ: xi =  x/z;  eta = -y/z;  break;
  }
  const double rho = (r - p->r_inner) / (p->r_outer - p->r_inner);
  return {xi, eta, rho};
}

//==============================================================
//  4. Patch POD (map + meta)
//==============================================================
struct Patch {
  PatchMap map;
  void*    meta;
};

//==============================================================
//  5. Fixed-size MultiPatch container
//==============================================================
template<std::size_t N>
class MultiPatch {
  Patch         patches_[N]{};
  std::size_t   count_{0};

  HD static constexpr bool valid(const Coord& l)
  {
    return (l[0] >= -1.1 && l[0] <= 1.1) &&
           (l[1] >= -1.1 && l[1] <= 1.1) &&
           (l[2] >= -1.1 && l[2] <= 1.1);
  }
public:
  HD std::size_t add_patch(const Patch& p)
  {
    return (count_ < N) ? (patches_[count_] = p, count_++) : N;
  }

  HD std::size_t size() const { return count_; }

  HD Coord local_to_global(std::size_t id, const Coord& l) const
  {
    return patches_[id].map.local_to_global(l, patches_[id].meta);
  }

  HD Coord global_to_local(const Coord& g, std::size_t& id_out) const
  {
    for (std::size_t i = 0; i < count_; ++i) {
      Coord l = patches_[i].map.global_to_local(g, patches_[i].meta);
      if (valid(l)) { id_out = i; return l; }
    }
    id_out = static_cast<std::size_t>(-1);
    return {0,0,0};
  }
};

//==============================================================
//  6. Factory helpers (no dynamic allocation)
//==============================================================
inline MultiPatch<1> make_cartesian_patch()
{
  static CartesianMeta meta{};
  Patch p{{cart_local_to_global, cart_global_to_local}, &meta};
  MultiPatch<1> mp; mp.add_patch(p); return mp;
}

inline MultiPatch<1> make_spherical_patch(double r_min, double r_max)
{
  static SphericalMeta meta{r_min, r_max};
  Patch p{{sph_local_to_global, sph_global_to_local}, &meta};
  MultiPatch<1> mp; mp.add_patch(p); return mp;
}

inline MultiPatch<7> make_multipatch_cube_sphere(double core_size,
                                                 double r_in, double r_out)
{
  (void)core_size; // placeholder: cube edge length not used in mapping
  MultiPatch<7> mp;

  //--- Cartesian core (id 0)
  static CartesianMeta core_meta{};
  Patch core{{cart_local_to_global, cart_global_to_local}, &core_meta};
  mp.add_patch(core);

  //--- Six wedges (ids 1-6)
  static CubedSphereMeta wedge_meta[6] = {
      {Face::PX, r_in, r_out}, {Face::NX, r_in, r_out},
      {Face::PY, r_in, r_out}, {Face::NY, r_in, r_out},
      {Face::PZ, r_in, r_out}, {Face::NZ, r_in, r_out}
  };
  for (std::size_t i = 0; i < 6; ++i) {
    Patch w{{cube_local_to_global, cube_global_to_local}, &wedge_meta[i]};
    mp.add_patch(w);
  }
  return mp;
}

//==============================================================
//  7. Usage examples (compile-time only; remove in production)
//==============================================================
/*
auto mp_cart = make_cartesian_patch();
Coord g1 = mp_cart.local_to_global(0, {0.1, 0.2, 0.3});

auto mp_sph = make_spherical_patch(1.0, 2.0);
Coord g2 = mp_sph.local_to_global(0, {0.5, M_PI/4, M_PI/3});

auto mp_cube = make_multipatch_cube_sphere(1.0, 1.0, 2.0);
Coord g3 = mp_cube.local_to_global(2, {0.0, 0.0, 0.5}); // +Y wedge
*/

