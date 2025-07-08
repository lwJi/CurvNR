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
CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr std::array<T, 9>
calc_jacSinC_inC(const std::array<T, 3> &xC) noexcept {
  const T x = xC[0], y = xC[1], z = xC[2];
  const T r2 = x*x + y*y + z*z;
  const T rh2 = x*x + y*y;
  const T r = sqrt(r2);
  const T rh = sqrt(rh2);
  const T rInv = T{1}/r;
  const T rInv2 = T{1}/r2;
  const T rhInv = T{1}/rh;
  const T rhInv2 = T{1}/rh2;
  return {
    rInv*x,
    rInv*y,
    rInv*z,
    rhInv*rInv2*x*z,
    rhInv*rInv2*y*z,
    -(rh*rInv2),
    -(rhInv2*y),
    rhInv2*x,
    0
  };
}

template <typename T>
CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr std::array<T, 9>
calc_jacSinC_inS(const std::array<T, 3> &xS) noexcept {
  const T r = xS[0], th = xS[1], ph = xS[2];
  const T st = std::sin(th)
  const T ct = std::cos(th)
  const T sp = std::sin(ph)
  const T cp = std::cos(ph)
  const T rInv = T{1}/r;
  const T stInv = T{1}/st;
  return {
    cp*st,
    sp*st,
    ct,
    cp*ct*rInv,
    ct*rInv*sp,
    -(rInv*st),
    -(rInv*sp*stInv),
    cp*rInv*stInv,
    0
  };
}

template <typename T>
CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr std::array<std::array<T, 9>, 3>
calc_djacSinC_inC(const std::array<T, 3> &xC) {
  const T x = xC[0], y = xC[1], z = xC[2];
  return {
    {
      rInv3*(Power(y,2) + Power(z,2)),
      -(rInv3*x*y),
      -(rInv3*x*z),
      rhInv3*rInv4*z*(-2*Power(x,4) - Power(x,2)*Power(y,2) + Power(y,4) + Power(y,2)*Power(z,2)),
      -(rhInv3*rInv4*x*y*z*(3*Power(x,2) + 3*Power(y,2) + Power(z,2))),
      rhInv*rInv4*x*(Power(rh,2) - Power(z,2)),
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
      rhInv*rInv4*y*(Power(rh,2) - Power(z,2)),
      rhInv4*(-Power(x,2) + Power(y,2)),
      -2*rhInv4*x*y,
      0
    },
    {
      -(rInv3*x*z),
      -(rInv3*y*z),
      Power(rh,2)*rInv3,
      rhInv*rInv4*x*(Power(rh,2) - Power(z,2)),
      rhInv*rInv4*y*(Power(rh,2) - Power(z,2)),
      2*rh*rInv4*z,
      0,
      0,
      0
    }
  };
}

template <typename T>
CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr std::array<std::array<T, 9>, 3>
calc_djacSinC_inS(const std::array<T, 3> &xS) {
  const T r = xS[0], th = xS[1], ph = xS[2];
  const T st = std::sin(th)
  const T ct = std::cos(th)
  const T sp = std::sin(ph)
  const T cp = std::cos(ph)
  const T rInv = T{1}/r;
  const T stInv = T{1}/st;
  return {
    {
      rInv*(Power(ct,2) + Power(sp,2)*Power(st,2)),
      -(cp*rInv*sp*Power(st,2)),
      -(cp*ct*rInv*st),
      cott*(-c2p + c2t*Power(cp,2))*rInv2,
      ((-2 + c2t)*cott*rInv2*s2p)/2.,
      -(c2t*cp*rInv2),
      rInv2*s2p*Power(stInv,2),
      -(c2p*rInv2*Power(stInv,2)),
      0
    },
    {
      -(cp*rInv*sp*Power(st,2)),
      rInv*(Power(ct,2) + Power(cp,2)*Power(st,2)),
      -(ct*rInv*sp*st),
      ((-2 + c2t)*cott*rInv2*s2p)/2.,
      cott*rInv2*(c2p + c2t*Power(sp,2)),
      -(c2t*rInv2*sp),
      -(c2p*rInv2*Power(stInv,2)),
      -(rInv2*s2p*Power(stInv,2)),
      0
    },
    {
      -(cp*ct*rInv*st),
      -(ct*rInv*sp*st),
      rInv*Power(st,2),
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
