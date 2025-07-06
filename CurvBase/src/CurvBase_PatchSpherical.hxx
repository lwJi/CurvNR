#ifndef CURVBASE_PATCHSPHERICAL_HXX
#define CURVBASE_PATCHSPHERICAL_HXX

#include <cmath>
#include <numbers>

#include "CurvBase.hxx"

namespace CurvBase {

constexpr CCTK_REAL twopi = 2.0 * M_PI;

struct SphericalMeta {
  CCTK_REAL r_min, r_max, dr;
  constexpr SphericalMeta(CCTK_REAL r0, CCTK_REAL r1) noexcept
      : r_min{r0}, r_max{r1}, dr{r1 - r0} {
    assert(r1 > r0 && "SphericalMeta: r_max must exceed r_min");
  }
};

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline Coord
sph_l2g(const Coord &l, const void *m) noexcept {
  const auto *p = static_cast<const SphericalMeta *>(m);
  const CCTK_REAL rho = l[0];                         // [0,1]
  const CCTK_REAL theta = l[1];                       // [0,2π]
  const CCTK_REAL phi = l[2];                         // [0,π]
  const CCTK_REAL r = std::fma(rho, p->dr, p->r_min); // r = rho * dr + r_min

  using std::sin, std::cos;
  const CCTK_REAL sinph = sin(phi);
  return {r * sinph * cos(theta), r * sinph * sin(theta), r * cos(phi)};
}

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline Coord
sph_g2l(const Coord &g, const void *m) noexcept {
  const auto *p = static_cast<const SphericalMeta *>(m);
  const CCTK_REAL x = g[0], y = g[1], z = g[2];

  const CCTK_REAL r = std::sqrt(x * x + y * y + z * z);
  const CCTK_REAL rho = (r - p->r_min) / p->dr;

  // atan2 returns (-π, π]; wrap to [0, 2π) for consistency
  using std::atan2, std::acos;
  CCTK_REAL theta = atan2(y, x);
  if (theta < 0.0)
    theta += twopi;

  // Clamp argument to avoid NaNs when |z| ≈ r due to FP noise
  const CCTK_REAL cosphi = z / r;
  const CCTK_REAL phi = acos(std::clamp(cosphi, -1.0, 1.0));

  return {rho, theta, phi};
}

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline bool
sph_valid(const Coord &l, const void *m) noexcept {
  const auto *p = static_cast<const SphericalMeta *>(m);
  return (l[0] >= 0.0 && l[0] <= 1.0) &&  // ρ ∈ [0,1]
         (l[1] >= 0.0 && l[1] < twopi) && // θ ∈ [0,2π)
         (l[2] >= 0.0 && l[2] <= M_PI);   // φ ∈ [0,π]
}

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCHSPHERICAL_HXX
