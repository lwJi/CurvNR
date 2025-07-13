#ifndef CURVBASE_PATCHCYLINDRICAL_HXX
#define CURVBASE_PATCHCYLINDRICAL_HXX

#include <cmath>
#include <numbers>

#include "CurvBase.hxx"

#include "../wolfram/JacobianCart2Cyl.hxx"

namespace CurvBase {

struct CylindricalMeta {};

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline Coord
cyl_l2g(const Coord &l, const void *) noexcept {
  const CCTK_REAL rho = l[0], phi = l[1], z = l[2];

  const CCTK_REAL sinph = std::sin(phi);
  const CCTK_REAL cosph = std::cos(phi);

  return {rho * cosph, rho * sinph, z};
}

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline Coord
cyl_g2l(const Coord &g, const void *) noexcept {
  const CCTK_REAL x = g[0], y = g[1], z = g[2];

  const CCTK_REAL rho = std::sqrt(std::fma(x, x, y * y));

  CCTK_REAL phi = std::atan2(y, x);
  if (phi < 0.0) {
    phi += twopi;
  }

  return {rho, phi, z};
}

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline bool
cyl_valid(const Coord &l, const void *) noexcept {
  return true;
}

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCHCYLINDRICAL_HXX
