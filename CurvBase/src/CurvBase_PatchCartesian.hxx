#ifndef CURVBASE_PATCHCARTESIAN_HXX
#define CURVBASE_PATCHCARTESIAN_HXX

#include "CurvBase.hxx"

namespace CurvBase {

struct CartesianMeta {};

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline Coord
cart_l2g(const Coord &l, const void *) noexcept {
  return l;
}

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline Coord
cart_g2l(const Coord &g, const void *) noexcept {
  return g;
}

[[nodiscard]] CCTK_HOST CCTK_DEVICE inline bool
cart_valid(const Coord &, const void *) noexcept {
  return true;
}

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCHCARTESIAN_HXX
