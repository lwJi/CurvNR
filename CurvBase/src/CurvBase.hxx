#ifndef CURVBASE_HXX
#define CURVBASE_HXX

#include <array>

#include <loop_device.hxx>

namespace CurvBase {
using namespace Loop;

constexpr CCTK_REAL onepi = M_PI;
constexpr CCTK_REAL twopi = 2.0 * M_PI;

using Index = std::array<CCTK_INT, dim>;
using Coord = std::array<CCTK_REAL, dim>;

} // namespace CurvBase

#endif // #ifndef CURVBASE_HXX
