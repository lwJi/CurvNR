/* JacobianCart2Sph.hxx */
/* Produced with Generato */

#ifndef JACOBIANCART2CUBEDSPHEREWEDGE_HXX
#define JACOBIANCART2CUBEDSPHEREWEDGE_HXX

#include <loop_device.hxx>

#include <array>
#include <cmath>

namespace CurvBase {
using namespace Loop;

template <typename T>
CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr std::array<T, 9>
jac_cart2wedge_cart(const std::array<T, 3> &xC) noexcept {
  return {0, 0, 0, 0, 0, 0, 0, 0, 0};
}

template <typename T>
CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr std::array<T, 9>
jac_cart2wedge_wedge(const std::array<T, 3> &xW) noexcept {
  return {0, 0, 0, 0, 0, 0, 0, 0, 0};
}

template <typename T>
CCTK_DEVICE CCTK_HOST
    CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr std::array<std::array<T, 9>, 3>
    djac_cart2wedge_cart(const std::array<T, 3> &xC) {
  return {{{0, 0, 0, 0, 0, 0, 0, 0, 0},
           {0, 0, 0, 0, 0, 0, 0, 0, 0},
           {0, 0, 0, 0, 0, 0, 0, 0, 0}}};
}

template <typename T>
CCTK_DEVICE CCTK_HOST
    CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr std::array<std::array<T, 9>, 3>
    djac_cart2wedge_wedge(const std::array<T, 3> &xW) {
  return {{{0, 0, 0, 0, 0, 0, 0, 0, 0},
           {0, 0, 0, 0, 0, 0, 0, 0, 0},
           {0, 0, 0, 0, 0, 0, 0, 0, 0}}};
}

} // namespace CurvBase

#endif // #ifndef JACOBIANCART2CUBEDSPHEREWEDGE_HXX

/* JacobianCart2Sph.hxx */
