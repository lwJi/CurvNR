/* JacobianCart2Sph.hxx */
/* Produced with Generato */

#ifndef JACOBIANCART2SPH_HXX
#define JACOBIANCART2SPH_HXX

#include <loop_device.hxx>

#include <array>
#include <cmath>

#include "powerinline.hxx"

namespace CurvBase {
using namespace Loop;

template <typename T>
CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE inline constexpr std::array<T, 9>
calc_jacSinC_inC(const std::array<T, 3> &xC) noexcept {
  const T &x = xC[0], &y = xC[1], &z = xC[2];
  return {
    rInv1*x,
    rInv1*y,
    rInv1*z,
    rhInv1*rInv2*x*z,
    rhInv1*rInv2*y*z,
    -(rh*rInv2),
    -(rhInv2*y),
    rhInv2*x,
    0
  };
}

template <typename T>
CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE inline constexpr std::array<T, 9>
calc_jacSinC_inS(const std::array<T, 3> &xS) noexcept {
  const T &r = xS[0], &th = xS[1], &ph = xS[2];
  const T st = std::sin(th)
  const T ct = std::cos(th)
  const T sp = std::sin(ph)
  const T cp = std::cos(ph)
  return {
    cp*st,
    sp*st,
    ct,
    cp*ct*rInv1,
    ct*rInv1*sp,
    -(rInv1*st),
    -(rInv1*sp*stInv),
    cp*rInv1*stInv,
    0
  };
}

} // namespace CurvBase

#endif // #ifndef JACOBIANCART2SPH_HXX

/* JacobianCart2Sph.hxx */
