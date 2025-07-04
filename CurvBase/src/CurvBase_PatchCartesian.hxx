#ifndef CURVBASE_PATCHCARTESIAN
#define CURVBASE_PATCHCARTESIAN

namespace CurvBase {

struct CartesianMeta {};

CCTK_HOST CCTK_DEVICE inline Coord cart_l2g(const Coord &l, const void *) {
  return l;
}

CCTK_HOST CCTK_DEVICE inline Coord cart_g2l(const Coord &g, const void *) {
  return g;
}

CCTK_HOST CCTK_DEVICE inline bool cart_valid(const Coord &) { return true; }

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCHCARTESIAN
