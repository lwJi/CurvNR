#ifndef CURVBASE_PATCHSPHERICAL_HXX
#define CURVBASE_PATCHSPHERICAL_HXX

#include <cmath>
#include <numbers>

#include "CurvBase.hxx"

namespace CurvBase {

struct SphericalMeta {};

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline Coord
sph_l2g(const Coord &l, const void *m) noexcept {
  const CCTK_REAL r = l[0];
  const CCTK_REAL theta = l[1]; // Polar angle [0, π]
  const CCTK_REAL phi = l[2];   // Azimuthal angle [0, 2π]

  using std::sin, std::cos;
  const CCTK_REAL sinth = sin(theta);
  return {r * sinth * cos(phi), r * sinth * sin(phi), r * cos(theta)};
}

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline Coord
sph_g2l(const Coord &g, const void *m) noexcept {
  const CCTK_REAL x = g[0], y = g[1], z = g[2];
  const CCTK_REAL r = std::sqrt(x * x + y * y + z * z);
  if (r == 0.0)
    return {0, 0, 0}; // Handle origin singularity

  // atan2(y, x) returns (-π, π]; wrap to [0, 2π) for consistency
  using std::atan2, std::acos;
  CCTK_REAL phi = atan2(y, x); // Azimuthal
  if (phi < 0.0)
    phi += twopi;

  // Clamp argument to acos to avoid NaNs from floating point noise
  const CCTK_REAL costheta = z / r;
  const CCTK_REAL theta = acos(std::clamp(costheta, -1.0, 1.0)); // Polar

  return {r, theta, phi};
}

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline bool
sph_valid(const Coord &l, const void *m) noexcept {
  return true;
}

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCHSPHERICAL_HXX
