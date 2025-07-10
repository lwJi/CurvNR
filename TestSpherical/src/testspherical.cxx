#include <curvderivs.hxx>
#include <loop_device.hxx>
#include <stx_derivsGF3D5.hxx>
#include <stx_powerinline.hxx>
#include <stx_utils.hxx>

#include <cctk.h>
#include <cctk_Arguments.h>
#include <cctk_Parameters.h>

#include <array>
#include <cassert>
#include <cmath>

namespace TestSpherical {
using namespace Loop;
using namespace CurvDerivs;
using namespace STXUtils;

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
          const CCTK_REAL x = ccoordx(p.I) - gaussian_shift_x;
          const CCTK_REAL y = ccoordy(p.I) - gaussian_shift_y;
          const CCTK_REAL z = ccoordz(p.I) - gaussian_shift_z;
          gaussian(amplitude, gaussian_width, cctk_time, x, y, z, u(p.I),
                   rho(p.I));
        });
  } else {
    CCTK_ERROR("Unknown initial condition");
  }
}

extern "C" void TestSpherical_RHS(CCTK_ARGUMENTS) {
  DECLARE_CCTK_ARGUMENTS_TestSpherical_RHS;
  DECLARE_CCTK_PARAMETERS;

#if 1

  constexpr int deriv_order = 4;

  for (int d = 0; d < 3; ++d)
    if (cctk_nghostzones[d] < deriv_order / 2)
      CCTK_VERROR("Need at least %d ghost zones", deriv_order / 2);

  const array<CCTK_REAL, 3> invDxyz{1. / CCTK_DELTA_SPACE(0),
                                    1. / CCTK_DELTA_SPACE(1),
                                    1. / CCTK_DELTA_SPACE(2)};

  const GF3D2layout layout2(cctkGH, {1, 1, 1});

  const array<const CCTK_REAL *, 9> gf_Jac{cJ1x, cJ1y, cJ1z, cJ2x, cJ2y,
                                           cJ2z, cJ3x, cJ3y, cJ3z};
  const array<const CCTK_REAL *, 18> gf_dJac{
      cdJ1xx, cdJ1xy, cdJ1xz, cdJ1yy, cdJ1yz, cdJ1zz, cdJ2xx, cdJ2xy, cdJ2xz,
      cdJ2yy, cdJ2yz, cdJ2zz, cdJ3xx, cdJ3xy, cdJ3xz, cdJ3yy, cdJ3yz, cdJ3zz};

  // allocate temporary GF3D5 gfs
  const int ntmps = 18;
  int itmp = 0;
  const GF3D5layout layout5 = STXUtils::get_GF3D5layout<1, 1, 1>(cctkGH);
  STXUtils::GF3D5Factory<CCTK_REAL> fct(layout5, ntmps, itmp);
  const auto tl_duSph = fct.make_vec_gf();
  const auto tl_dduSph = fct.make_smat_gf();
  const auto tl_duCart = fct.make_vec_gf();
  const auto tl_dduCart = fct.make_smat_gf();
  if (itmp != ntmps)
    CCTK_VERROR("Wrong number of temporary variables: ntmps=%d itmp=%d", ntmps,
                itmp);
  itmp = -1;

  const Loop::GridDescBaseDevice grid(cctkGH);

  // Derivatives of Spherical Coordinate
  calc_derivs2nd<1, 1, 1>(grid, layout5, tl_duSph, tl_dduSph, layout2, u,
                          invDxyz, deriv_order);

  // Transformation from Spherical to Cartesian Coordinate
  calc_trans<1, 1, 1>(grid, layout5, tl_duCart, tl_dduCart, tl_duSph, tl_dduSph,
                      layout2, gf_Jac, gf_dJac);

  const auto dduCart11 = tl_dduCart[0].ptr;
  const auto dduCart22 = tl_dduCart[3].ptr;
  const auto dduCart33 = tl_dduCart[5].ptr;

  grid.loop_int_device<1, 1, 1>(
      grid.nghostzones,
      [=] CCTK_DEVICE(const Loop::PointDesc &p) CCTK_ATTRIBUTE_ALWAYS_INLINE {
        const int ijk = layout2.linear(p.i, p.j, p.k);
        const int ijk5 = layout5.linear(p.i, p.j, p.k);

        u_rhs[ijk] = rho[ijk];
        rho_rhs[ijk] = dduCart11[ijk5] + dduCart22[ijk5] + dduCart33[ijk];
      });

#else

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

        if (std::isnan(rho_rhs(p.I))) {
          printf(
              "iter = %i, xyz = %16.8e, %16.8e, %16.8e,   ijk = %i, %i, %i,\n",
              cctk_iteration, p.x, p.y, p.z, p.i, p.j, p.k);
          assert(0);
        }
      });

#endif
}

extern "C" void TestSpherical_Sync(CCTK_ARGUMENTS) {
  // do nothing
}

} // namespace TestSpherical
