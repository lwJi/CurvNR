#ifndef CURVBASE_PATCHSPHERICAL_HXX
#define CURVBASE_PATCHSPHERICAL_HXX

#include "CurvBase.hxx"

namespace CurvBase {

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
  const double r = std::sqrt(x * x + y * y + z * z);
  return {(r - p->r_min) / (p->r_max - p->r_min), // ρ
          std::atan2(y, x),                       // theta
          std::acos(z / r)};                      // phi
}

CCTK_HOST CCTK_DEVICE inline bool sph_valid(const Coord &l) {
  return (l[0] >= 0 && l[0] <= 1) && (l[1] >= 0 && l[1] <= 2 * M_PI) &&
         (l[2] >= 0 && l[2] <= M_PI);
}

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCHSPHERICAL_HXX
