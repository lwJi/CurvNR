#ifndef CURVBASE_HXX
#define CURVBASE_HXX

#include <array>

#include <loop_device.hxx>

namespace CurvBase {
using namespace Loop;

constexpr CCTK_REAL onepi = M_PI;
constexpr CCTK_REAL twopi = 2.0 * M_PI;

struct FaceInfo {
  bool is_outer_boundary{true};
};

constexpr FaceInfo outer_face{true};
constexpr FaceInfo inner_face{false};

using Index = std::array<CCTK_INT, dim>;
using Coord = std::array<CCTK_REAL, dim>;
using Jac_t = std::array<CCTK_REAL, 9>;
using dJac_t = std::array<std::array<CCTK_REAL, 9>, 3>;
using PatchFaces = std::array<std::array<FaceInfo, dim>, 2>;

} // namespace CurvBase

#endif // #ifndef CURVBASE_HXX
