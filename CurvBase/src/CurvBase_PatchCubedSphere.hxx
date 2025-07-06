#ifndef CURVBASE_PATCHCUBEDSPHERE_HXX
#define CURVBASE_PATCHCUBEDSPHERE_HXX

#include "CurvBase.hxx"

namespace CurvBase {

enum class Wedge { PX, NX, PY, NY, PZ, NZ };

struct CubedSphereMeta {
  Wedge wedge;                        // which wedge
  CCTK_REAL r_inner, r_outer, inv_dr; // radial extent of the wedge
  CCTK_HOST constexpr CubedSphereMeta(Wedge w, CCTK_REAL r0,
                                      CCTK_REAL r1) noexcept
      : wedge{w}, r_inner{r0}, r_outer{r1}, inv_dr{CCTK_REAL{1} / (r1 - r0)} {}
};

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline Coord
cubedsphere_l2g(const Coord &l, const void *m) noexcept {
  const auto *p = static_cast<const CubedSphereMeta *>(m);
  const CCTK_REAL xi = l[0], eta = l[1], rho = l[2];
  const CCTK_REAL r = std::fma(rho, (p->r_outer - p->r_inner), p->r_inner);
  const CCTK_REAL d = std::sqrt(CCTK_REAL{1} + xi * xi + eta * eta);

  switch (p->wedge) {
  case Wedge::PX:
    return {r / d, r * xi / d, r * eta / d};
  case Wedge::NX:
    return {-r / d, -r * xi / d, r * eta / d};
  case Wedge::PY:
    return {-r * xi / d, r / d, r * eta / d};
  case Wedge::NY:
    return {r * xi / d, -r / d, r * eta / d};
  case Wedge::PZ:
    return {r * xi / d, r * eta / d, r / d};
  case Wedge::NZ:
    return {r * xi / d, -r * eta / d, -r / d};
  }
  return {0, 0, 0}; // unreachable

  // Replace six-way switch with a tiny look-up + sign matrix – no
  // divergence.
  // constexpr int perm[6][3] = {{0, 1, 2}, {0, 1, 2}, {1, 0, 2},
  //                             {1, 0, 2}, {0, 1, 2}, {0, 1, 2}};
  // constexpr int sgn[6][3] = {{+1, +1, +1}, {-1, -1, +1}, {-1, +1, +1},
  //                            {+1, -1, +1}, {+1, +1, +1}, {+1, -1, -1}};
  // const int f = static_cast<int>(p->wedge);
  // const Coord v{r / d, r * xi / d, r * eta / d};
  // return {sgn[f][0] * v[perm[f][0]], sgn[f][1] * v[perm[f][1]],
  //         sgn[f][2] * v[perm[f][2]]};
}

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline Coord
cubedsphere_g2l(const Coord &g, const void *m) noexcept {
  const auto *p = static_cast<const CubedSphereMeta *>(m);
  const CCTK_REAL x = g[0], y = g[1], z = g[2];
  const CCTK_REAL r = std::sqrt(x * x + y * y + z * z);

  CCTK_REAL xi = 0, eta = 0;
  switch (p->wedge) {
  case Wedge::PX:
    xi = y / x;
    eta = z / x;
    break;
  case Wedge::NX:
    xi = -y / x;
    eta = z / x;
    break;
  case Wedge::PY:
    xi = -x / y;
    eta = z / y;
    break;
  case Wedge::NY:
    xi = x / y;
    eta = z / y;
    break;
  case Wedge::PZ:
    xi = x / z;
    eta = y / z;
    break;
  case Wedge::NZ:
    xi = x / z;
    eta = -y / z;
    break;
  }
  const CCTK_REAL rho = (r - p->r_inner) * p->inv_dr;
  return {xi, eta, rho};
}

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline bool
cubedsphere_valid(const Coord &l, const void *m) noexcept {
  return (l[0] >= -1 && l[0] <= 1) && (l[1] >= -1 && l[1] <= 1) &&
         (l[2] >= 0 && l[2] <= 1);
}

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCHCUBEDSPHERE_HXX
