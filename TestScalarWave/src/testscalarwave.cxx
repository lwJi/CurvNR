#include <curvtrans.hxx>
#include <curvtransderivs.hxx>
#include <cx_derivsGF3D5.hxx>
#include <cx_powerinline.hxx>
#include <cx_utils.hxx>
#include <loop_device.hxx>

#include <cctk.h>
#include <cctk_Arguments.h>
#include <cctk_Parameters.h>

#include <array>
#include <cassert>
#include <cmath>

namespace TestScalarWave {
using namespace Loop;
using namespace std;

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

extern "C" void TestScalarWave_Initial(CCTK_ARGUMENTS) {
  DECLARE_CCTK_ARGUMENTSX_TestScalarWave_Initial;
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

extern "C" void TestScalarWave_RHS(CCTK_ARGUMENTS) {
  DECLARE_CCTK_ARGUMENTS_TestScalarWave_RHS;
  DECLARE_CCTK_PARAMETERS;

  for (int d = 0; d < 3; ++d)
    if (cctk_nghostzones[d] < deriv_order / 2)
      CCTK_VERROR("Need at least %d ghost zones", deriv_order / 2);

  const array<CCTK_REAL, 3> invDxyz{1. / CCTK_DELTA_SPACE(0),
                                    1. / CCTK_DELTA_SPACE(1),
                                    1. / CCTK_DELTA_SPACE(2)};

  const GF3D2layout layout2(cctkGH, {1, 1, 1});
  const GF3D5layout layout5 = CXUtils::get_GF3D5layout<1, 1, 1>(cctkGH);

  const array<const CCTK_REAL *, 9> gf_Jac{cJ1x, cJ1y, cJ1z, cJ2x, cJ2y,
                                           cJ2z, cJ3x, cJ3y, cJ3z};
  const array<const CCTK_REAL *, 18> gf_dJac{
      cdJ1xx, cdJ1xy, cdJ1xz, cdJ1yy, cdJ1yz, cdJ1zz, cdJ2xx, cdJ2xy, cdJ2xz,
      cdJ2yy, cdJ2yz, cdJ2zz, cdJ3xx, cdJ3xy, cdJ3xz, cdJ3yy, cdJ3yz, cdJ3zz};

  // allocate temporary GF3D5 gfs
  const int ntmps = 9;
  int itmp = 0;
  CXUtils::GF3D5Factory<CCTK_REAL> fct(layout5, ntmps, itmp);
  const auto tl_du = fct.make_vec_gf();
  const auto tl_ddu = fct.make_smat_gf();
  if (itmp != ntmps)
    CCTK_VERROR("Wrong number of temporary variables: ntmps=%d itmp=%d", ntmps,
                itmp);
  itmp = -1;

  const Loop::GridDescBaseDevice grid(cctkGH);

  if (use_jacobian) {

    // Derivatives of Cartesian Coordinate
    switch (deriv_order) {
    case 2: {
      CurvDerivs::calc_transderivs<1, 1, 1, 2>(
          grid, layout5, tl_du, tl_ddu, layout2, u, invDxyz, gf_Jac, gf_dJac);
      break;
    }
    case 4: {
      CurvDerivs::calc_transderivs<1, 1, 1, 4>(
          grid, layout5, tl_du, tl_ddu, layout2, u, invDxyz, gf_Jac, gf_dJac);
      break;
    }
    case 6: {
      CurvDerivs::calc_transderivs<1, 1, 1, 6>(
          grid, layout5, tl_du, tl_ddu, layout2, u, invDxyz, gf_Jac, gf_dJac);
      break;
    }
    case 8: {
      CurvDerivs::calc_transderivs<1, 1, 1, 8>(
          grid, layout5, tl_du, tl_ddu, layout2, u, invDxyz, gf_Jac, gf_dJac);
      break;
    }
    default:
      assert(0 && "Invalid derivative order");
    }

    const auto ddu11 = tl_ddu[0].ptr;
    const auto ddu22 = tl_ddu[3].ptr;
    const auto ddu33 = tl_ddu[5].ptr;

    grid.loop_int_device<1, 1, 1>(
        grid.nghostzones,
        [=] CCTK_DEVICE(const Loop::PointDesc &p) CCTK_ATTRIBUTE_ALWAYS_INLINE {
          const int ijk = layout2.linear(p.i, p.j, p.k);
          const int ijk5 = layout5.linear(p.i, p.j, p.k);
          u_rhs[ijk] = rho[ijk];
          rho_rhs[ijk] = ddu11[ijk5] + ddu22[ijk5] + ddu33[ijk5];
        });

  } else {

    // Derivatives of Curvilinear Coordinate
    CXUtils::calc_derivs2nd<1, 1, 1>(grid, layout5, tl_du, tl_ddu, layout2, u,
                                     invDxyz, deriv_order);

    const auto du1 = tl_du[0].ptr;
    const auto du2 = tl_du[1].ptr;
    const auto du3 = tl_du[2].ptr;
    const auto ddu11 = tl_ddu[0].ptr;
    const auto ddu22 = tl_ddu[3].ptr;
    const auto ddu33 = tl_ddu[5].ptr;

    if (CCTK_EQUALS(patch_system, "Spherical")) {
      grid.loop_int_device<1, 1, 1>(
          grid.nghostzones,
          [=] CCTK_DEVICE(const Loop::PointDesc &p)
              CCTK_ATTRIBUTE_ALWAYS_INLINE {
                const int ijk = layout2.linear(p.i, p.j, p.k);
                const int ijk5 = layout5.linear(p.i, p.j, p.k);
                const auto r = p.x;
                const auto r2 = r * r;
                const auto sinth = std::sin(p.y);
                const auto costh = std::cos(p.y);
                const auto cotth = costh / sinth;
                const auto sinth2 = sinth * sinth;

                u_rhs[ijk] = rho[ijk];
                rho_rhs[ijk] = ddu11[ijk5] + 2 * du1[ijk5] / r +
                               (cotth * du2[ijk5] + ddu22[ijk5]) / r2 +
                               ddu33[ijk5] / (r2 * sinth2);
              });
    } else if (CCTK_EQUALS(patch_system, "Cylindrical")) {
      grid.loop_int_device<1, 1, 1>(
          grid.nghostzones,
          [=] CCTK_DEVICE(const Loop::PointDesc &p)
              CCTK_ATTRIBUTE_ALWAYS_INLINE {
                const int ijk = layout2.linear(p.i, p.j, p.k);
                const int ijk5 = layout5.linear(p.i, p.j, p.k);
                const auto rh = p.x;
                const auto rh2 = rh * rh;
                const auto sinph = std::sin(p.y);
                const auto cosph = std::cos(p.y);

                u_rhs[ijk] = rho[ijk];
                rho_rhs[ijk] = ddu11[ijk5] + du1[ijk5] / rh +
                               ddu22[ijk5] / rh2 + ddu33[ijk5];
              });
    } else {
      CCTK_ERROR("Unknown patch system");
    }
  }
}

extern "C" void TestScalarWave_Sync(CCTK_ARGUMENTS) {
  // do nothing
}

} // namespace TestScalarWave
