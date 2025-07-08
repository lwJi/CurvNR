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
CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE inline std::array<T, 9>
calc_jacSinC(const std::array<T, 3> &xC) {
  const T &x = xC[0], &y = xC[1], &z = xC[2];
  return {
    x/Sqrt(Power(x,2) + Power(y,2) + Power(z,2)),
    y/Sqrt(Power(x,2) + Power(y,2) + Power(z,2)),
    z/Sqrt(Power(x,2) + Power(y,2) + Power(z,2)),
    (x*z)/(Sqrt(Power(x,2) + Power(y,2))*(Power(x,2) + Power(y,2) + Power(z,2))),
    (y*z)/(Sqrt(Power(x,2) + Power(y,2))*(Power(x,2) + Power(y,2) + Power(z,2))),
    -(Sqrt(Power(x,2) + Power(y,2))/(Power(x,2) + Power(y,2) + Power(z,2))),
    -(y/(Power(x,2) + Power(y,2))),
    x/(Power(x,2) + Power(y,2)),
    0
  };
}

template <typename T>
CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE inline std::arrya<std::array<T, 9>, 3>
calc_djacSinC(const std::array<T, 3> &xC) {
  const T &x = xC[0], &y = xC[1], &z = xC[2];
  return {
    {
      -(Power(x,2)/Power(Power(x,2) + Power(y,2) + Power(z,2),1.5)) + 1/Sqrt(Power(x,2) + Power(y,2) + Power(z,2)),
      -((x*y)/Power(Power(x,2) + Power(y,2) + Power(z,2),1.5)),
      -((x*z)/Power(Power(x,2) + Power(y,2) + Power(z,2),1.5)),
      z/(Sqrt(Power(x,2) + Power(y,2))*(Power(x,2) + Power(y,2) + Power(z,2))) + x*((-2*x*z)/(Sqrt(Power(x,2) + Power(y,2))*Power(Power(x,2) + Power(y,2) + Power(z,2),2)) - (x*z)/(Power(Power(x,2) + Power(y,2),1.5)*(Power(x,2) + Power(y,2) + Power(z,2)))),
      y*((-2*x*z)/(Sqrt(Power(x,2) + Power(y,2))*Power(Power(x,2) + Power(y,2) + Power(z,2),2)) - (x*z)/(Power(Power(x,2) + Power(y,2),1.5)*(Power(x,2) + Power(y,2) + Power(z,2)))),
      (2*x*Sqrt(Power(x,2) + Power(y,2)))/Power(Power(x,2) + Power(y,2) + Power(z,2),2) - x/(Sqrt(Power(x,2) + Power(y,2))*(Power(x,2) + Power(y,2) + Power(z,2))),
      (2*x*y)/Power(Power(x,2) + Power(y,2),2),
      (-2*Power(x,2))/Power(Power(x,2) + Power(y,2),2) + 1/(Power(x,2) + Power(y,2)),
      0
    },
    {
      -((x*y)/Power(Power(x,2) + Power(y,2) + Power(z,2),1.5)),
      -(Power(y,2)/Power(Power(x,2) + Power(y,2) + Power(z,2),1.5)) + 1/Sqrt(Power(x,2) + Power(y,2) + Power(z,2)),
      -((y*z)/Power(Power(x,2) + Power(y,2) + Power(z,2),1.5)),
      x*((-2*y*z)/(Sqrt(Power(x,2) + Power(y,2))*Power(Power(x,2) + Power(y,2) + Power(z,2),2)) - (y*z)/(Power(Power(x,2) + Power(y,2),1.5)*(Power(x,2) + Power(y,2) + Power(z,2)))),
      z/(Sqrt(Power(x,2) + Power(y,2))*(Power(x,2) + Power(y,2) + Power(z,2))) + y*((-2*y*z)/(Sqrt(Power(x,2) + Power(y,2))*Power(Power(x,2) + Power(y,2) + Power(z,2),2)) - (y*z)/(Power(Power(x,2) + Power(y,2),1.5)*(Power(x,2) + Power(y,2) + Power(z,2)))),
      (2*y*Sqrt(Power(x,2) + Power(y,2)))/Power(Power(x,2) + Power(y,2) + Power(z,2),2) - y/(Sqrt(Power(x,2) + Power(y,2))*(Power(x,2) + Power(y,2) + Power(z,2))),
      (2*Power(y,2))/Power(Power(x,2) + Power(y,2),2) - 1/(Power(x,2) + Power(y,2)),
      (-2*x*y)/Power(Power(x,2) + Power(y,2),2),
      0
    },
    {
      -((x*z)/Power(Power(x,2) + Power(y,2) + Power(z,2),1.5)),
      -((y*z)/Power(Power(x,2) + Power(y,2) + Power(z,2),1.5)),
      -(Power(z,2)/Power(Power(x,2) + Power(y,2) + Power(z,2),1.5)) + 1/Sqrt(Power(x,2) + Power(y,2) + Power(z,2)),
      (x*((-2*Power(z,2))/Power(Power(x,2) + Power(y,2) + Power(z,2),2) + 1/(Power(x,2) + Power(y,2) + Power(z,2))))/Sqrt(Power(x,2) + Power(y,2)),
      (y*((-2*Power(z,2))/Power(Power(x,2) + Power(y,2) + Power(z,2),2) + 1/(Power(x,2) + Power(y,2) + Power(z,2))))/Sqrt(Power(x,2) + Power(y,2)),
      (2*Sqrt(Power(x,2) + Power(y,2))*z)/Power(Power(x,2) + Power(y,2) + Power(z,2),2),
      0,
      0,
      0
    }
  };
}

} // namespace CurvBase

#endif // #ifndef JACOBIANCART2SPH_HXX

/* JacobianCart2Sph.hxx */
