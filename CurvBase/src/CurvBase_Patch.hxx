#ifndef CURVBASE_PATCH_HXX
#define CURVBASE_PATCH_HXX

#include <array>
#include <cmath>

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

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCH_HXX
