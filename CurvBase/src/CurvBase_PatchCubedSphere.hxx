#ifndef CURVBASE_PATCHCUBEDSPHERE_HXX
#define CURVBASE_PATCHCUBEDSPHERE_HXX

#include "CurvBase.hxx"

namespace CurvBase {

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
  const double r = std::sqrt(x * x + y * y + z * z);

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

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCHCUBEDSPHERE_HXX
