#ifndef CURVBASE_PATCHSPHERICAL_HXX
#define CURVBASE_PATCHSPHERICAL_HXX

#include "CurvBase.hxx"

namespace CurvBase {

struct SphericalMeta {
  double r_min, r_max; // inner / outer radius
  SphericalMeta(double r0, double r1) noexcept : r_min{r0}, r_max{r1} {}
};

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline Coord
sph_l2g(const Coord &l, const void *m) noexcept {
  const auto *p = static_cast<const SphericalMeta *>(m);
  const double rho = l[0];   // [0,1]
  const double theta = l[1]; // [0,2π]
  const double phi = l[2];   // [0,π]
  const double r = p->r_min + rho * (p->r_max - p->r_min);
  return {r * std::sin(phi) * std::cos(theta),
          r * std::sin(phi) * std::sin(theta), r * std::cos(phi)};
}

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline Coord
sph_g2l(const Coord &g, const void *m) noexcept {
  const auto *p = static_cast<const SphericalMeta *>(m);
  const double x = g[0], y = g[1], z = g[2];
  const double r = std::sqrt(x * x + y * y + z * z);
  return {(r - p->r_min) / (p->r_max - p->r_min), // ρ
          std::atan2(y, x),                       // theta
          std::acos(z / r)};                      // phi
}

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline bool
sph_valid(const Coord &l, const void *m) noexcept {
  const auto *p = static_cast<const SphericalMeta *>(m);
  return (l[0] >= p->r_min && l[0] <= p->r_max) &&
         (l[1] >= 0 && l[1] <= 2 * M_PI) && (l[2] >= 0 && l[2] <= M_PI);
}

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCHSPHERICAL_HXX
