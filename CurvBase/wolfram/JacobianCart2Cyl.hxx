/* JacobianCart2Cyl.hxx */
/* Produced with Generato */

#ifndef JACOBIANCART2CYL_HXX
#define JACOBIANCART2CYL_HXX

#include <loop_device.hxx>

#include <array>
#include <cmath>

namespace CurvBase {
using namespace Loop;

template <typename T>
CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr std::array<std::array<T, 3>, 3>
jac_cart2cyl_cart(const std::array<T, 3> &xC) noexcept {
  const T x = xC[0], y = xC[1];

  const T x2 = x*x;
  const T y2 = y*y;

  const T rh2 = x2 + y2;
  const T rh = sqrt(rh2);

  const T rhInv = T{1}/rh;
  const T rhInv2 = rhInv*rhInv;

  return {{
    {
      rhInv*x,
      rhInv*y,
      0
    },
    {
      -(rhInv2*y),
      rhInv2*x,
      0
    },
    {
      0,
      0,
      1
    }
  }};
}

template <typename T>
CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr std::array<std::array<T, 3>, 3>
jac_cart2cyl_cyl(const std::array<T, 3> &xCyl) noexcept {
  const T rh = xCyl[0], ph = xCyl[1];

  const T sp = std::sin(ph);
  const T cp = std::cos(ph);

  const T rhInv = T{1}/rh;

  return {{
    {
      cp,
      sp,
      0
    },
    {
      -(rhInv*sp),
      cp*rhInv,
      0
    },
    {
      0,
      0,
      1
    }
  }};
}

template <typename T>
CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr std::array<std::array<T, 6>, 3>
djac_cart2cyl_cart(const std::array<T, 3> &xC) {
  const T x = xC[0], y = xC[1];

  const T x2 = x*x;
  const T y2 = y*y;

  const T rh2 = x2 + y2;
  const T rh = sqrt(rh2);

  const T rhInv = T{1}/rh;
  const T rhInv2 = rhInv*rhInv;
  const T rhInv3 = rhInv2*rhInv;
  const T rhInv4 = rhInv2*rhInv2;

  return {{
    {
      rhInv3*y2,
      -(rhInv3*x*y),
      0,
      rhInv3*x2,
      0,
      0
    },
    {
      2*rhInv4*x*y,
      rhInv4*(-x2 + y2),
      0,
      -2*rhInv4*x*y,
      0,
      0
    },
    {
      0,
      0,
      0,
      0,
      0,
      0
    }
  }};
}

template <typename T>
CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr std::array<std::array<T, 6>, 3>
djac_cart2cyl_cyl(const std::array<T, 3> &xCyl) {
  const T rh = xCyl[0], ph = xCyl[1];

  const T sp = std::sin(ph);
  const T cp = std::cos(ph);

  const T sp2 = sp*sp;
  const T cp2 = cp*cp;

  const T s2p = T{2}*cp*sp;
  const T c2p = cp2 - sp2;

  const T rhInv = T{1}/rh;
  const T rhInv2 = rhInv*rhInv;

  return {{
    {
      rhInv*sp2,
      -(cp*rhInv*sp),
      0,
      cp2*rhInv,
      0,
      0
    },
    {
      rhInv2*s2p,
      -(c2p*rhInv2),
      0,
      -(rhInv2*s2p),
      0,
      0
    },
    {
      0,
      0,
      0,
      0,
      0,
      0
    }
  }};
}

} // namespace CurvBase

#endif // #ifndef JACOBIANCART2CYL_HXX

/* JacobianCart2Cyl.hxx */
