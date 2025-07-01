#ifndef CURVBASE_PATCH_HXX
#define CURVBASE_PATCH_HXX

#include <array>
#include <cmath>

#include "CurvBase_PatchCartesian.hxx"
#include "CurvBase_PatchCubedSphere.hxx"
#include "CurvBase_PatchSpherical.hxx"

namespace CurvBase {

using Coord = std::array<double, 3>;

struct PatchMap {
  CCTK_HOST CCTK_DEVICE Coord (*local_to_global)(const Coord &,
                                                 const void *) = nullptr;
  CCTK_HOST CCTK_DEVICE Coord (*global_to_local)(const Coord &,
                                                 const void *) = nullptr;
  CCTK_HOST CCTK_DEVICE bool (*is_valid_local)(const Coord &) = nullptr;
};

struct Patch {
  PatchMap map;
  void *meta = nullptr;
};

// Helpers that create individual Patch objects
inline Patch make_cart_patch() {
  static CartesianMeta meta{};
  return Patch{{cart_local_to_global, cart_global_to_local, cart_valid}, &meta};
}
inline Patch make_sph_patch(double r0, double r1) {
  static SphericalMeta meta{r0, r1};
  return Patch{{sph_local_to_global, sph_global_to_local, sph_valid}, &meta};
}
inline Patch make_wedge(Face f, double r0, double r1) {
  static CubedSphereMeta meta{f, r0, r1};
  return Patch{{cubedsphere_local_to_global, cubedsphere_global_to_local,
                cubedsphere_valid},
               &meta};
}

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCH_HXX
