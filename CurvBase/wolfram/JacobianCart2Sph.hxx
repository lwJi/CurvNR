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

template <typename T>
CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE inline std::array<std::array<T, 9>, 3>
calc_djacSinC_inC(const std::array<T, 3> &xC) {
  const T &x = xC[0], &y = xC[1], &z = xC[2];
  return {
    {
      rInv3*(Power(y,2) + Power(z,2)),
      -(rInv3*x*y),
      -(rInv3*x*z),
      rhInv3*rInv4*z*(-2*Power(x,4) - Power(x,2)*Power(y,2) + Power(y,4) + Power(y,2)*Power(z,2)),
      -(rhInv3*rInv4*x*y*z*(3*Power(x,2) + 3*Power(y,2) + Power(z,2))),
      rhInv1*rInv4*x*(Power(rh,2) - Power(z,2)),
      2*rhInv4*x*y,
      rhInv4*(-Power(x,2) + Power(y,2)),
      0
    },
    {
      -(rInv3*x*y),
      rInv3*(Power(x,2) + Power(z,2)),
      -(rInv3*y*z),
      -(rhInv3*rInv4*x*y*z*(3*Power(x,2) + 3*Power(y,2) + Power(z,2))),
      rhInv3*rInv4*z*(Power(x,4) - 2*Power(y,4) + Power(x,2)*(-Power(y,2) + Power(z,2))),
      rhInv1*rInv4*y*(Power(rh,2) - Power(z,2)),
      rhInv4*(-Power(x,2) + Power(y,2)),
      -2*rhInv4*x*y,
      0
    },
    {
      -(rInv3*x*z),
      -(rInv3*y*z),
      Power(rh,2)*rInv3,
      rhInv1*rInv4*x*(Power(rh,2) - Power(z,2)),
      rhInv1*rInv4*y*(Power(rh,2) - Power(z,2)),
      2*rh*rInv4*z,
      0,
      0,
      0
    }
  };
}

template <typename T>
CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE inline std::array<std::array<T, 9>, 3>
calc_djacSinC_inS(const std::array<T, 3> &xS) {
  const T &x = xS[0], &y = xS[1], &z = xS[2];
  return {
    {
      rInv1*(Power(ct,2) + Power(sp,2)*Power(st,2)),
      -(cp*rInv1*sp*Power(st,2)),
      -(cp*ct*rInv1*st),
      cott*(-c2p + c2t*Power(cp,2))*rInv2,
      ((-2 + c2t)*cott*rInv2*s2p)/2.,
      -(c2t*cp*rInv2),
      rInv2*s2p*Power(stInv,2),
      -(c2p*rInv2*Power(stInv,2)),
      0
    },
    {
      -(cp*rInv1*sp*Power(st,2)),
      rInv1*(Power(ct,2) + Power(cp,2)*Power(st,2)),
      -(ct*rInv1*sp*st),
      ((-2 + c2t)*cott*rInv2*s2p)/2.,
      cott*rInv2*(c2p + c2t*Power(sp,2)),
      -(c2t*rInv2*sp),
      -(c2p*rInv2*Power(stInv,2)),
      -(rInv2*s2p*Power(stInv,2)),
      0
    },
    {
      -(cp*ct*rInv1*st),
      -(ct*rInv1*sp*st),
      rInv1*Power(st,2),
      -(c2t*cp*rInv2),
      -(c2t*rInv2*sp),
      rInv2*s2t,
      0,
      0,
      0
    }
  };
}

} // namespace CurvBase

#endif // #ifndef JACOBIANCART2SPH_HXX

/* JacobianCart2Sph.hxx */
