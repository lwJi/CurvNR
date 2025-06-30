//======================================================================
//  MultiPatch — single-header runtime library
//  • Cartesian single patch
//  • Spherical shell single patch
//  • Cubed-sphere composite (Cartesian core + 6 wedges)
//
//  - Host/Device ready    (all public callables tagged with HD)
//  - No dynamic allocation (all meta data stored statically)
//  - Patch–local “valid()” now lives in PatchMap
//======================================================================
#pragma once
#include <array>
#include <cmath>
#include <cstddef>   // std::size_t

//----------------------------------
//  Convenience host/device tag
//----------------------------------
#ifndef HD
#  define HD __host__ __device__
#endif

//----------------------------------
//  Basic helpers & typedefs
//----------------------------------
using Coord = std::array<double,3>;
template<class T> HD constexpr T sq(T x) { return x * x; }

//======================================================================
//  1.  Per-patch metadata (“meta”)
//======================================================================
struct CartesianMeta { /* add bounds later if desired */ };

struct SphericalMeta {
  double r_min, r_max;                    // inner / outer radius
};

enum class Face { PX, NX, PY, NY, PZ, NZ };

struct CubedSphereMeta {
  Face   face;                            // cube face
  double r_in, r_out;                     // radial extent
};

//======================================================================
//  2.  PatchMap  (ops + validity test)
//======================================================================
struct PatchMap
{
  HD Coord (*local_to_global)(const Coord&, const void*) = nullptr;
  HD Coord (*global_to_local)(const Coord&, const void*) = nullptr;
  HD bool  (*valid_local)     (const Coord&, const void*) = nullptr;
};

//======================================================================
//  3.  Concrete mapping + validity callbacks
//======================================================================
//-- Cartesian ----------------------------------------------------------
HD inline Coord cart_l2g(const Coord& l,const void*)           { return l; }
HD inline Coord cart_g2l(const Coord& g,const void*)           { return g; }
HD inline bool  cart_valid(const Coord&,const void*)           { return true; }

//-- Spherical shell ----------------------------------------------------
HD inline Coord sph_l2g(const Coord& l,const void* m)
{
  const auto* p = static_cast<const SphericalMeta*>(m);
  const double ρ   = l[0];
  const double θ   = l[1];
  const double φ   = l[2];
  const double r   = p->r_min + ρ*(p->r_max - p->r_min);
  return { r*std::sin(φ)*std::cos(θ),
           r*std::sin(φ)*std::sin(θ),
           r*std::cos(φ) };
}
HD inline Coord sph_g2l(const Coord& g,const void* m)
{
  const auto* p = static_cast<const SphericalMeta*>(m);
  const double x=g[0], y=g[1], z=g[2];
  const double r = std::sqrt(sq(x)+sq(y)+sq(z));
  return { (r-p->r_min)/(p->r_max-p->r_min),
           std::atan2(y,x),
           std::acos(z/r) };
}
HD inline bool sph_valid(const Coord& l,const void*)
{
  return (l[0]>=0.0 && l[0]<=1.0) &&
         (l[1]>=0.0 && l[1]<=2*M_PI) &&
         (l[2]>=0.0 && l[2]<=M_PI);
}

//-- Cubed-sphere wedge --------------------------------------------------
HD inline Coord cube_l2g(const Coord& l,const void* m)
{
  const auto* p = static_cast<const CubedSphereMeta*>(m);
  const double ξ   = l[0], η = l[1], ρ = l[2];
  const double r   = p->r_in + ρ*(p->r_out - p->r_in);
  const double d   = std::sqrt(1.0 + ξ*ξ + η*η);

  switch (p->face) {
    case Face::PX: return {  r/d        ,  r*ξ/d       ,  r*η/d };
    case Face::NX: return { -r/d        , -r*ξ/d       ,  r*η/d };
    case Face::PY: return { -r*ξ/d      ,  r/d         ,  r*η/d };
    case Face::NY: return {  r*ξ/d      , -r/d         ,  r*η/d };
    case Face::PZ: return {  r*ξ/d      ,  r*η/d       ,  r/d   };
    case Face::NZ: return {  r*ξ/d      , -r*η/d       , -r/d   };
  }
  return {0,0,0};
}
HD inline Coord cube_g2l(const Coord& g,const void* m)
{
  const auto* p = static_cast<const CubedSphereMeta*>(m);
  const double x=g[0], y=g[1], z=g[2];
  const double r = std::sqrt(sq(x)+sq(y)+sq(z));

  double ξ=0, η=0;
  switch (p->face) {
    case Face::PX: ξ= y/x;   η= z/x;   break;
    case Face::NX: ξ=-y/x;   η= z/x;   break;
    case Face::PY: ξ=-x/y;   η= z/y;   break;
    case Face::NY: ξ= x/y;   η= z/y;   break;
    case Face::PZ: ξ= x/z;   η= y/z;   break;
    case Face::NZ: ξ= x/z;   η=-y/z;   break;
  }
  const double ρ = (r - p->r_in) / (p->r_out - p->r_in);
  return {ξ, η, ρ};
}
HD inline bool cube_valid(const Coord& l,const void*)
{
  return (l[0]>=-1.0&&l[0]<=1.0) &&
         (l[1]>=-1.0&&l[1]<=1.0) &&
         (l[2]>= 0.0&&l[2]<=1.0);
}

//======================================================================
//  4.  Patch POD  (map + meta pointer)
//======================================================================
struct Patch {
  PatchMap map{};
  void*    meta = nullptr;
};

//======================================================================
//  5.  Fixed-size MultiPatch container
//======================================================================
template<std::size_t N>
class MultiPatch
{
  Patch       patches_[N]{};
  std::size_t count_{0};

public:
  HD std::size_t add_patch(const Patch& p)
  { return (count_<N)?(patches_[count_]=p,count_++):N; }

  HD std::size_t size() const { return count_; }

  HD Coord local_to_global(std::size_t id,const Coord& l) const
  { return patches_[id].map.local_to_global(l, patches_[id].meta); }

  HD Coord global_to_local(const Coord& g,std::size_t& id_out) const
  {
    for(std::size_t i=0;i<count_;++i) {
      Coord l = patches_[i].map.global_to_local(g, patches_[i].meta);
      if(patches_[i].map.valid_local &&
         patches_[i].map.valid_local(l, patches_[i].meta)) {
        id_out = i; return l;
      }
    }
    id_out = static_cast<std::size_t>(-1);
    return {0,0,0};
  }
};

//======================================================================
//  6.  Factory helpers  (no dynamic allocation)
//======================================================================
//-- Single Cartesian block ---------------------------------------------
inline MultiPatch<1> make_cartesian_patch()
{
  static CartesianMeta meta{};
  Patch p{{cart_l2g, cart_g2l, cart_valid}, &meta};
  MultiPatch<1> mp; mp.add_patch(p); return mp;
}

//-- Single spherical shell ---------------------------------------------
inline MultiPatch<1> make_spherical_patch(double r_min,double r_max)
{
  static SphericalMeta meta{r_min,r_max};
  Patch p{{sph_l2g, sph_g2l, sph_valid}, &meta};
  MultiPatch<1> mp; mp.add_patch(p); return mp;
}

//-- Cubed-sphere composite (1 core + 6 wedges) -------------------------
inline MultiPatch<7> make_multipatch_cube_sphere(double cube_size,
                                                 double r_in,double r_out)
{
  (void)cube_size;  // not used yet (mapping is identity inside cube)
  MultiPatch<7> mp;

  // Cartesian core (id 0)
  static CartesianMeta core_meta{};
  Patch core{{cart_l2g, cart_g2l, cart_valid}, &core_meta};
  mp.add_patch(core);

  // Six wedges (ids 1–6)
  static CubedSphereMeta wedge_meta[6] = {
      {Face::PX,r_in,r_out},{Face::NX,r_in,r_out},
      {Face::PY,r_in,r_out},{Face::NY,r_in,r_out},
      {Face::PZ,r_in,r_out},{Face::NZ,r_in,r_out}};
  for(std::size_t i=0;i<6;++i){
    Patch w{{cube_l2g, cube_g2l, cube_valid}, &wedge_meta[i]};
    mp.add_patch(w);
  }
  return mp;
}

//======================================================================
//  7.  Tiny usage snippet  (remove in production)
//======================================================================
/*
auto mp_cart = make_cartesian_patch();
Coord g0 = mp_cart.local_to_global(0,{0.1,0.2,0.3});

auto mp_sph = make_spherical_patch(1.0,2.0);
Coord g1 = mp_sph.local_to_global(0,{0.5,M_PI/4,M_PI/3});

auto mp_cube = make_multipatch_cube_sphere(1.0,1.0,2.0);
Coord g2 = mp_cube.local_to_global(2,{0.0,0.0,0.5});   // +Y wedge
*/

