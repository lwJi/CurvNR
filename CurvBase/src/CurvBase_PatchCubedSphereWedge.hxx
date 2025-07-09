#ifndef CURVBASE_PATCHCUBEDSPHEREWEDGE_HXX
#define CURVBASE_PATCHCUBEDSPHEREWEDGE_HXX

#include "CurvBase.hxx"

#include "../wolfram/JacobianCart2CubedSphereWedge.hxx"

namespace CurvBase {

enum class Wedge { PX, NX, PY, NY, PZ, NZ };

struct CubedSphereWedgeMeta {
  Wedge wedge;
  CCTK_REAL r_inner, r_outer, inv_dr;
  CCTK_HOST constexpr CubedSphereWedgeMeta(Wedge w, CCTK_REAL r0,
                                           CCTK_REAL r1) noexcept
      : wedge{w}, r_inner{r0}, r_outer{r1}, inv_dr{CCTK_REAL{1} / (r1 - r0)} {}
};

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline Coord
cubedspherewedge_l2g(const Coord &l, const void *m) noexcept {
  const auto *p = static_cast<const CubedSphereWedgeMeta *>(m);
  const CCTK_REAL xi = l[0], eta = l[1], rho = l[2];

  const CCTK_REAL r = std::fma(rho, (p->r_outer - p->r_inner), p->r_inner);

  // const CCTK_REAL d = std::sqrt(CCTK_REAL{1} + xi * xi + eta * eta);
  // switch (p->wedge) {
  // case Wedge::PX:
  //   return {r / d, r * xi / d, r * eta / d};
  // case Wedge::NX:
  //   return {-r / d, -r * xi / d, r * eta / d};
  // case Wedge::PY:
  //   return {-r * xi / d, r / d, r * eta / d};
  // case Wedge::NY:
  //   return {r * xi / d, -r / d, r * eta / d};
  // case Wedge::PZ:
  //   return {r * xi / d, r * eta / d, r / d};
  // case Wedge::NZ:
  //   return {r * xi / d, -r * eta / d, -r / d};
  // }

  const CCTK_REAL r_div_d = r / std::sqrt(CCTK_REAL{1} + xi * xi + eta * eta);

  const Coord comp = {r_div_d, xi * r_div_d, eta * r_div_d};
  const int f = static_cast<int>(p->wedge);

  // This table encodes the permutations and sign flips from the original
  // switch. Accessing this table is a constant-time operation with no
  // branching. Rows correspond to Wedge enum. Columns encode: {sgn_x,
  // perm_idx_x, sgn_y, perm_idx_y, sgn_z, perm_idx_z} perm_idx: 0 -> comp[0], 1
  // -> comp[1], 2 -> comp[2]
  static constexpr CCTK_REAL tbl[6][6] = {/* PX */ {+1.0, 0, +1.0, 1, +1.0, 2},
                                          /* NX */ {-1.0, 0, -1.0, 1, +1.0, 2},
                                          /* PY */ {-1.0, 1, +1.0, 0, +1.0, 2},
                                          /* NY */ {+1.0, 1, -1.0, 0, +1.0, 2},
                                          /* PZ */ {+1.0, 1, +1.0, 2, +1.0, 0},
                                          /* NZ */ {+1.0, 1, -1.0, 2, -1.0, 0}};

  return {tbl[f][0] * comp[static_cast<int>(tbl[f][1])],
          tbl[f][2] * comp[static_cast<int>(tbl[f][3])],
          tbl[f][4] * comp[static_cast<int>(tbl[f][5])]};
}

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline Coord
cubedspherewedge_g2l(const Coord &g, const void *m) noexcept {
  const auto *p = static_cast<const CubedSphereWedgeMeta *>(m);
  const CCTK_REAL x = g[0], y = g[1], z = g[2];

  // const CCTK_REAL r = std::sqrt(x * x + y * y + z * z);
  // CCTK_REAL xi = 0, eta = 0;
  // switch (p->wedge) {
  // case Wedge::PX:
  //   xi = y / x;
  //   eta = z / x;
  //   break;
  // case Wedge::NX:
  //   xi = -y / x;
  //   eta = z / x;
  //   break;
  // case Wedge::PY:
  //   xi = -x / y;
  //   eta = z / y;
  //   break;
  // case Wedge::NY:
  //   xi = x / y;
  //   eta = z / y;
  //   break;
  // case Wedge::PZ:
  //   xi = x / z;
  //   eta = y / z;
  //   break;
  // case Wedge::NZ:
  //   xi = x / z;
  //   eta = -y / z;
  //   break;
  // }

  // Fused multiply-add can be more accurate and faster.
  const CCTK_REAL r_sq = std::fma(x, x, std::fma(y, y, z * z));
  const CCTK_REAL r = std::sqrt(r_sq);

  const int f = static_cast<int>(p->wedge);

  // Using arrays indexed by the wedge index `f` avoids a divergent switch.
  // All threads will execute the same division instructions, just with
  // different data.
  const CCTK_REAL denoms[] = {x, x, y, y, z, z};
  const CCTK_REAL xi_nums[] = {y, -y, -x, x, x, x};
  const CCTK_REAL eta_nums[] = {z, z, z, z, y, -y};

  const CCTK_REAL xi = xi_nums[f] / denoms[f];
  const CCTK_REAL eta = eta_nums[f] / denoms[f];

  const CCTK_REAL rho = (r - p->r_inner) * p->inv_dr;
  return {xi, eta, rho};
}

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline bool
cubedspherewedge_valid(const Coord &l, const void *) noexcept {
  // Allow for a small tolerance for floating point inaccuracies at boundaries.
  constexpr CCTK_REAL tol = 1.0e-12;
  return (l[0] >= -1.0 - tol && l[0] <= 1.0 + tol) &&
         (l[1] >= -1.0 - tol && l[1] <= 1.0 + tol) &&
         (l[2] >= 0.0 - tol && l[2] <= 1.0 + tol);
}

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCHCUBEDSPHEREWEDGE_HXX
