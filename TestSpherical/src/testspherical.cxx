#include <loop_device.hxx>

#include <cctk.h>
#include <cctk_Arguments.h>
#include <cctk_Parameters.h>

#include <array>
#include <cassert>
#include <cmath>

namespace TestSpherical {
using namespace Loop;

// u(t,r) = (f(t-r) - f(t+r)) / r
// f(v) = A exp(-1/2 (r/W)^2)
template <typename T>
constexpr void gaussian(const T A, const T W, const T t, const T x, const T y,
                        const T z, T &u, T &rho) {
  using std::exp, std::pow, std::sqrt;

  const T r = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
  const auto f = [&](const T v) {
    return A * exp(-pow(v, 2) / (2 * pow(W, 2)));
  };

  if (r < sqrt(std::numeric_limits<T>::epsilon())) {
    // L'Hôpital
    u = 2 / pow(W, 2) * f(t) * t;
    rho = -2 / pow(W, 4) * f(t) * (pow(t, 2) - pow(W, 2));
  } else {
    u = (f(t - r) - f(t + r)) / r;
    rho = -(f(t - r) * (t - r) - f(t + r) * (t + r)) / (pow(W, 2) * r);
  }
}

template <typename T>
constexpr void sinewave(const T A, const T freq, const T t, const T x,
                        const T y, const T z, const T kx, const T ky,
                        const T kz, T &u, T &rho) {
  using std::sin, std::cos;

  const T dot = kx * x + ky * y + kz * z;
  const T omega = 2.0 * M_PI * freq;

  u = A * sin(2.0 * M_PI * dot - omega * t);
  rho = -A * omega * cos(2.0 * M_PI * dot - omega * t);
}

extern "C" void TestSpherical_Initial(CCTK_ARGUMENTS) {
  DECLARE_CCTK_ARGUMENTSX_TestSpherical_Initial;
  DECLARE_CCTK_PARAMETERS;

  if (CCTK_EQUALS(initial_condition, "Sine Wave")) {
    grid.loop_int_device<1, 1, 1>(
        grid.nghostzones,
        [=] CCTK_DEVICE(const Loop::PointDesc &p) CCTK_ATTRIBUTE_ALWAYS_INLINE {
          const CCTK_REAL kx = 0.1;
          const CCTK_REAL ky = 0.1;
          const CCTK_REAL kz = 0.1;
          const CCTK_REAL freq = 1.0;
          sinewave(amplitude, freq, cctk_time, ccoordx(p.I), ccoordy(p.I),
                   ccoordz(p.I), kx, ky, kz, u(p.I), rho(p.I));
        });
  } else if (CCTK_EQUALS(initial_condition, "Gaussian")) {
    grid.loop_int_device<1, 1, 1>(
        grid.nghostzones,
        [=] CCTK_DEVICE(const Loop::PointDesc &p) CCTK_ATTRIBUTE_ALWAYS_INLINE {
          gaussian(amplitude, gaussian_width, cctk_time, ccoordx(p.I),
                   ccoordy(p.I), ccoordz(p.I), u(p.I), rho(p.I));
        });
  } else {
    CCTK_ERROR("Unknown initial condition");
  }
}

extern "C" void TestSpherical_RHS(CCTK_ARGUMENTS) {
  DECLARE_CCTK_ARGUMENTSX_TestSpherical_RHS;
  DECLARE_CCTK_PARAMETERS;

  grid.loop_int_device<1, 1, 1>(
      grid.nghostzones,
      [=] CCTK_DEVICE(const Loop::PointDesc &p) CCTK_ATTRIBUTE_ALWAYS_INLINE {
        CCTK_REAL r = p.x;
        CCTK_REAL r2 = r * r;
        CCTK_REAL sinth = std::sin(p.y);
        CCTK_REAL costh = std::cos(p.y);
        CCTK_REAL cotth = costh / sinth;
        CCTK_REAL sinth2 = sinth * sinth;

        std::array<CCTK_REAL, dim> du, ddu;
        for (int d = 0; d < dim; ++d) {
          du[d] = ((u(p.I - 2 * p.DI[d]) - u(p.I + 2 * p.DI[d])) -
                   8 * (u(p.I - p.DI[d]) - u(p.I + p.DI[d]))) /
                  (12 * p.DX[d]);
          ddu[d] = (-(u(p.I - 2 * p.DI[d]) + u(p.I + 2 * p.DI[d])) +
                    16 * (u(p.I - p.DI[d]) + u(p.I + p.DI[d])) - 30 * u(p.I)) /
                   (12 * pow(p.DX[d], 2));
        }

        u_rhs(p.I) = rho(p.I);
        rho_rhs(p.I) = ddu[0] + 2 * du[0] / r + (cotth * du[1] + ddu[1]) / r2 +
                       ddu[2] / (r2 * sinth2);
      });
}

extern "C" void TestSpherical_Sync(CCTK_ARGUMENTS) {
  // do nothing
}

} // namespace TestSpherical
