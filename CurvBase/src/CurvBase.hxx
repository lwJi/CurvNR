#ifndef CURVBASE_HXX
#define CURVBASE_HXX

#include <array>

namespace CurvBase {
using namespace Loop;

using Index = std::array<CCTK_INT, dim>;
using Coord = std::array<CCTK_REAL, dim>;

} // namespace CurvBase

#endif // #ifndef CURVBASE_HXX
