/* JacobianCart2Sph.hxx */
/* Produced with Generato */

#ifndef JACOBIANCART2SPH_HXX
#define JACOBIANCART2SPH_HXX

#include <loop_device.hxx>

#include <array>
#include <cmath>

namespace CurvBase {
using namespace Loop;

template <typename T>
CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr std::array<std::array<T, 3>, 3>
jac_cart2sph_cart(const std::array<T, 3> &xC) noexcept {
  const T x = xC[0], y = xC[1], z = xC[2];

  const T x2 = x*x;
  const T y2 = y*y;
  const T z2 = z*z;

  const T r2 = x2 + y2 + z2;
  const T r = sqrt(r2);
  const T rh2 = x2 + y2;
  const T rh = sqrt(rh2);

  const T rInv = T{1}/r;
  const T rInv2 = rInv*rInv;
  const T rhInv = T{1}/rh;
  const T rhInv2 = rhInv*rhInv;

  return {{
    {
      rInv*x,
      rInv*y,
      rInv*z
    },
    {
      rhInv*rInv2*x*z,
      rhInv*rInv2*y*z,
      -(rh*rInv2)
    },
    {
      -(rhInv2*y),
      rhInv2*x,
      0
    }
  }};
}

template <typename T>
CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr std::array<std::array<T, 3>, 3>
jac_cart2sph_sph(const std::array<T, 3> &xS) noexcept {
  const T r = xS[0], th = xS[1], ph = xS[2];

  const T st = std::sin(th);
  const T ct = std::cos(th);
  const T sp = std::sin(ph);
  const T cp = std::cos(ph);

  const T rInv = T{1}/r;
  const T stInv = T{1}/st;

  return {{
    {
      cp*st,
      sp*st,
      ct
    },
    {
      cp*ct*rInv,
      ct*rInv*sp,
      -(rInv*st)
    },
    {
      -(rInv*sp*stInv),
      cp*rInv*stInv,
      0
    }
  }};
}

template <typename T>
CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr std::array<std::array<T, 6>, 3>
djac_cart2sph_cart(const std::array<T, 3> &xC) {
  const T x = xC[0], y = xC[1], z = xC[2];

  const T x2 = x*x;
  const T x4 = x2*x2;
  const T y2 = y*y;
  const T y4 = y2*y2;
  const T z2 = z*z;

  const T r2 = x2 + y2 + z2;
  const T r = sqrt(r2);
  const T rh2 = x2 + y2;
  const T rh = sqrt(rh2);

  const T rInv = T{1}/r;
  const T rInv2 = rInv*rInv;
  const T rInv3 = rInv2*rInv;
  const T rInv4 = rInv2*rInv2;
  const T rhInv = T{1}/rh;
  const T rhInv2 = rhInv*rhInv;
  const T rhInv3 = rhInv2*rhInv;
  const T rhInv4 = rhInv2*rhInv2;

  return {{
    {
      rInv3*(y2 + z2),
      -(rInv3*x*y),
      -(rInv3*x*z),
      rInv3*(x2 + z2),
      -(rInv3*y*z),
      rh2*rInv3
    },
    {
      rhInv3*rInv4*z*(-2*x4 - x2*y2 + y4 + y2*z2),
      -(rhInv3*rInv4*x*y*z*(3*x2 + 3*y2 + z2)),
      rhInv*rInv4*x*(rh2 - z2),
      rhInv3*rInv4*z*(x4 - 2*y4 + x2*(-y2 + z2)),
      rhInv*rInv4*y*(rh2 - z2),
      2*rh*rInv4*z
    },
    {
      2*rhInv4*x*y,
      rhInv4*(-x2 + y2),
      0,
      -2*rhInv4*x*y,
      0,
      0
    }
  }};
}

template <typename T>
CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr std::array<std::array<T, 6>, 3>
djac_cart2sph_sph(const std::array<T, 3> &xS) {
  const T r = xS[0], th = xS[1], ph = xS[2];

  const T st = std::sin(th);
  const T ct = std::cos(th);
  const T sp = std::sin(ph);
  const T cp = std::cos(ph);

  const T st2 = st*st;
  const T ct2 = ct*ct;
  const T sp2 = sp*sp;
  const T cp2 = cp*cp;

  const T s2t = T{2}*ct*st;
  const T c2t = ct2 - st2;
  const T s2p = T{2}*cp*sp;
  const T c2p = cp2 - sp2;
  const T cott = ct/st;

  const T rInv = T{1}/r;
  const T rInv2 = rInv*rInv;
  const T stInv = T{1}/st;
  const T stInv2 = stInv*stInv;

  return {{
    {
      rInv*(ct2 + sp2*st2),
      -(cp*rInv*sp*st2),
      -(cp*ct*rInv*st),
      rInv*(ct2 + cp2*st2),
      -(ct*rInv*sp*st),
      rInv*st2
    },
    {
      cott*(-c2p + c2t*cp2)*rInv2,
      ((-2 + c2t)*cott*rInv2*s2p)/2.,
      -(c2t*cp*rInv2),
      cott*rInv2*(c2p + c2t*sp2),
      -(c2t*rInv2*sp),
      rInv2*s2t
    },
    {
      rInv2*s2p*stInv2,
      -(c2p*rInv2*stInv2),
      0,
      -(rInv2*s2p*stInv2),
      0,
      0
    }
  }};
}

} // namespace CurvBase

#endif // #ifndef JACOBIANCART2SPH_HXX

/* JacobianCart2Sph.hxx */
