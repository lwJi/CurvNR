#ifndef CURVBASE_PATCHSPHERICAL_HXX
#define CURVBASE_PATCHSPHERICAL_HXX

#include <cmath>
#include <numbers>

#include "CurvBase.hxx"

namespace CurvBase {

struct SphericalMeta {};

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline Coord
sph_l2g(const Coord &l, const void *) noexcept {
  const CCTK_REAL r = l[0], theta = l[1], phi = l[2];

  // On GPUs, calling a `sincos` intrinsic to compute sine and cosine
  // simultaneously is often faster than separate calls.
  // Example: sincos(theta, &sin_theta, &cos_theta);
  const CCTK_REAL sinth = std::sin(theta);
  const CCTK_REAL costh = std::cos(theta);
  const CCTK_REAL sinph = std::sin(phi);
  const CCTK_REAL cosph = std::cos(phi);

  return {r * sinth * cosph, r * sinth * sinph, r * costh};
}

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline Coord
sph_g2l(const Coord &g, const void *) noexcept {
  const CCTK_REAL x = g[0], y = g[1], z = g[2];
  const CCTK_REAL r_sq = std::fma(x, x, std::fma(y, y, z * z));
  const CCTK_REAL r = std::sqrt(r_sq);

  if (r == 0.0)
    return {0, 0, 0};

  // atan2 handles signs correctly and is more stable than atan(y/x)
  CCTK_REAL phi = std::atan2(y, x);
  if (phi < 0.0)
    phi += twopi;

  // Clamp argument to acos to prevent NaNs from floating-point error
  const CCTK_REAL safe_arg = std::clamp(z / r, -1.0, 1.0);
  const CCTK_REAL theta = std::acos(safe_arg);

  return {r, theta, phi};
}

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline bool
sph_valid(const Coord &, const void *) noexcept {
  return true;
}

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCHSPHERICAL_HXX
