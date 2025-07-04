#ifndef CURVBASE_PATCH_HXX
#define CURVBASE_PATCH_HXX

#include <array>
#include <cmath>

#include "CurvBase_PatchCartesian.hxx"
#include "CurvBase_PatchCubedSphere.hxx"
#include "CurvBase_PatchSpherical.hxx"

namespace CurvBase {

using Coord = std::array<CCTK_REAL, 3>;

//------------------------------------------------------------------------------
// Cartesian Patch
//------------------------------------------------------------------------------
struct CartesianMeta {};

CCTK_HOST CCTK_DEVICE inline Coord cart_l2g(const Coord &l, const void *) {
  return l;
}

CCTK_HOST CCTK_DEVICE inline Coord cart_g2l(const Coord &g, const void *) {
  return g;
}

CCTK_HOST CCTK_DEVICE inline bool cart_valid(const Coord &) { return true; }

//------------------------------------------------------------------------------
// CubedSphere Patch
//------------------------------------------------------------------------------
enum class Face { PX, NX, PY, NY, PZ, NZ };

struct CubedSphereMeta {
  Face face;               // which cube face
  double r_inner, r_outer; // radial extent of the wedge
};

CCTK_HOST CCTK_DEVICE inline Coord cubedsphere_l2g(const Coord &l,
                                                   const void *m) {
  const auto *p = static_cast<const CubedSphereMeta *>(m);
  const double xi = l[0], eta = l[1], rho = l[2];
  const double r = p->r_inner + rho * (p->r_outer - p->r_inner);
  const double d = std::sqrt(1.0 + xi * xi + eta * eta);

  switch (p->face) {
  case Face::PX:
    return {r / d, r * xi / d, r * eta / d};
  case Face::NX:
    return {-r / d, -r * xi / d, r * eta / d};
  case Face::PY:
    return {-r * xi / d, r / d, r * eta / d};
  case Face::NY:
    return {r * xi / d, -r / d, r * eta / d};
  case Face::PZ:
    return {r * xi / d, r * eta / d, r / d};
  case Face::NZ:
    return {r * xi / d, -r * eta / d, -r / d};
  }
  return {0, 0, 0}; // unreachable
}

CCTK_HOST CCTK_DEVICE inline Coord cubedsphere_g2l(const Coord &g,
                                                   const void *m) {
  const auto *p = static_cast<const CubedSphereMeta *>(m);
  const double x = g[0], y = g[1], z = g[2];
  const double r = std::sqrt(sq(x) + sq(y) + sq(z));

  double xi = 0, eta = 0;
  switch (p->face) {
  case Face::PX:
    xi = y / x;
    eta = z / x;
    break;
  case Face::NX:
    xi = -y / x;
    eta = z / x;
    break;
  case Face::PY:
    xi = -x / y;
    eta = z / y;
    break;
  case Face::NY:
    xi = x / y;
    eta = z / y;
    break;
  case Face::PZ:
    xi = x / z;
    eta = y / z;
    break;
  case Face::NZ:
    xi = x / z;
    eta = -y / z;
    break;
  }
  const double rho = (r - p->r_inner) / (p->r_outer - p->r_inner);
  return {xi, eta, rho};
}

CCTK_HOST CCTK_DEVICE inline bool cubedsphere_valid(const Coord &l) {
  return (l[0] >= -1 && l[0] <= 1) && (l[1] >= -1 && l[1] <= 1) &&
         (l[2] >= 0 && l[2] <= 1);
}

//------------------------------------------------------------------------------
// Spherical Patch
//------------------------------------------------------------------------------
struct SphericalMeta {
  double r_min, r_max; // inner / outer radius
};

CCTK_HOST CCTK_DEVICE inline Coord sph_l2g(const Coord &l, const void *m) {
  const auto *p = static_cast<const SphericalMeta *>(m);
  const double rho = l[0];   // [0,1]
  const double theta = l[1]; // [0,2π]
  const double phi = l[2];   // [0,π]
  const double r = p->r_min + rho * (p->r_max - p->r_min);
  return {r * std::sin(phi) * std::cos(theta),
          r * std::sin(phi) * std::sin(theta), r * std::cos(phi)};
}

CCTK_HOST CCTK_DEVICE inline Coord sph_g2l(const Coord &g, const void *m) {
  const auto *p = static_cast<const SphericalMeta *>(m);
  const double x = g[0], y = g[1], z = g[2];
  const double r = std::sqrt(sq(x) + sq(y) + sq(z));
  return {(r - p->r_min) / (p->r_max - p->r_min), // ρ
          std::atan2(y, x),                       // theta
          std::acos(z / r)};                      // phi
}

CCTK_HOST CCTK_DEVICE inline bool sph_valid(const Coord &l) {
  return (l[0] >= 0 && l[0] <= 1) && (l[1] >= 0 && l[1] <= 2 * M_PI) &&
         (l[2] >= 0 && l[2] <= M_PI);
}

//------------------------------------------------------------------------------
// Patch
//------------------------------------------------------------------------------
struct PatchMap {
  CCTK_HOST CCTK_DEVICE Coord (*l2g)(const Coord &, const void *) = nullptr;
  CCTK_HOST CCTK_DEVICE Coord (*g2l)(const Coord &, const void *) = nullptr;
  CCTK_HOST CCTK_DEVICE bool (*is_valid)(const Coord &) = nullptr;
};

struct FaceInfo {
  bool is_outer_boundary{true};
};

struct Patch {
  // metadata container
  std::variant<CartesianMeta, SphericalMeta, CubedSphereMeta> meta{};

  // helper: return pointer to the **concrete** meta inside the variant
  const void *meta_ptr() const noexcept {
    return std::visit([](auto const &m) -> const void * { return &m; }, meta);
  }

  PatchMap map{};

  std::array<CCTK_INT, dim> ncells{}; // cells per dimension
  std::array<CCTK_REAL, dim> xmin{};  // lower physical bounds
  std::array<CCTK_REAL, dim> xmax{};  // upper physical bounds

  std::array<std::array<FaceInfo, dim>, 2> faces{};

  bool is_cartesian{false};
};

//------------------------------------------------------------------------------
// Helpers that create individual Patch objects
//------------------------------------------------------------------------------
inline Patch make_cart_patch() {
  Patch p;
  p.map = {cart_l2g, cart_g2l, cart_valid};
  p.meta = CartesianMeta{}; // active alt set
  p.is_cartesian = true;
  return p;
}

inline Patch make_sph_patch(CCTK_REAL r0, CCTK_REAL r1) {
  Patch p;
  p.map = {sph_l2g, sph_g2l, sph_valid};
  p.meta = SphericalMeta{r0, r1};
  p.is_cartesian = false;
  return p;
}

inline Patch make_wedge(Face f, CCTK_REAL r0, CCTK_REAL r1) {
  Patch p;
  p.map = {cubedsphere_l2g, cubedsphere_g2l, cubedsphere_valid};
  p.meta = CubedSphereMeta{f, r0, r1};
  p.is_cartesian = false;
  return p;
}

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCH_HXX
