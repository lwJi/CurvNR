#ifndef JACOBIANCART2CART_HXX
#define JACOBIANCART2CART_HXX

#include <loop_device.hxx>

#include <array>
#include <cmath>

namespace CurvBase {
using namespace Loop;

template <typename T>
CCTK_DEVICE CCTK_HOST
    CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr std::array<std::array<T, 3>, 3>
    jac_cart2cart_cart(const std::array<T, 3> &xC) noexcept {
  return {{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}};
}

template <typename T>
CCTK_DEVICE CCTK_HOST
    CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr std::array<std::array<T, 6>, 3>
    djac_cart2cart_cart(const std::array<T, 3> &xC) {
  return {{{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}}};
}

} // namespace CurvBase

#endif // #ifndef JACOBIANCART2CART_HXX

/* JacobianCart2Sph.hxx */
