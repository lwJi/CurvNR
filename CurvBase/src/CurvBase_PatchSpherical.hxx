#ifndef CURVBASE_PATCHSPHERICAL_HXX
#define CURVBASE_PATCHSPHERICAL_HXX

#include <cmath>
#include <numbers>

#include "CurvBase.hxx"

namespace CurvBase {

struct SphericalMeta {};

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline Coord
sph_l2g(const Coord &l, const void *m) noexcept {
  const CCTK_REAL r = l[0];     // [0,Infty)
  const CCTK_REAL theta = l[1]; // [0,π]
  const CCTK_REAL phi = l[2];   // [0,2π]

  using std::sin, std::cos;
  const CCTK_REAL sinph = sin(phi);
  return {r * sinph * cos(theta), r * sinph * sin(theta), r * cos(phi)};
}

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline Coord
sph_g2l(const Coord &g, const void *m) noexcept {
  const CCTK_REAL x = g[0], y = g[1], z = g[2];
  const CCTK_REAL r = std::sqrt(x * x + y * y + z * z);

  // atan2 returns (-π, π]; wrap to [0, 2π) for consistency
  using std::atan2, std::acos;
  CCTK_REAL theta = atan2(y, x);
  if (theta < 0.0)
    theta += twopi;

  // Clamp argument to avoid NaNs when |z| ≈ r due to FP noise
  const CCTK_REAL cosphi = z / r;
  const CCTK_REAL phi = acos(std::clamp(cosphi, -1.0, 1.0));

  return {r, theta, phi};
}

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline bool
sph_valid(const Coord &l, const void *m) noexcept {
  return true;
}

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCHSPHERICAL_HXX
