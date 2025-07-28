#include <loop.hxx>

#include <cctk.h>
#include <cctk_Arguments.h>
#include <cctk_Parameters.h>

#include <cmath>

namespace CurvADM {
using namespace Loop;
using namespace std;

extern "C" void CurvADM_linear_wave(CCTK_ARGUMENTS) {
  DECLARE_CCTK_ARGUMENTSX_CurvADM_linear_wave;
  DECLARE_CCTK_PARAMETERS;

  const CCTK_REAL t = cctk_time;

  // See arXiv:1111.2177 [gr-qc], (74-75)

  const auto b = [&](const PointDesc &p) {
    return linear_wave_amplitude *
           sin(2 * CCTK_REAL(M_PI) * (p.x - t) / linear_wave_wavelength);
  };

  const auto bt = [&](const PointDesc &p) {
    return -2 * CCTK_REAL(M_PI) * linear_wave_amplitude /
           linear_wave_wavelength *
           cos(2 * CCTK_REAL(M_PI) * (p.x - t) / linear_wave_wavelength);
  };

  loop_all<1, 1, 1>(cctkGH, [&](const PointDesc &p) { gxx(p.I) = 1; });
  loop_all<1, 1, 1>(cctkGH, [&](const PointDesc &p) { gxy(p.I) = 0; });
  loop_all<1, 1, 1>(cctkGH, [&](const PointDesc &p) { gxz(p.I) = 0; });
  loop_all<1, 1, 1>(cctkGH, [&](const PointDesc &p) { gyy(p.I) = 1 + b(p); });
  loop_all<1, 1, 1>(cctkGH, [&](const PointDesc &p) { gyz(p.I) = 0; });
  loop_all<1, 1, 1>(cctkGH, [&](const PointDesc &p) { gzz(p.I) = 1 - b(p); });

  loop_all<1, 1, 1>(cctkGH, [&](const PointDesc &p) { kxx(p.I) = 0; });
  loop_all<1, 1, 1>(cctkGH, [&](const PointDesc &p) { kxy(p.I) = 0; });
  loop_all<1, 1, 1>(cctkGH, [&](const PointDesc &p) { kxz(p.I) = 0; });
  loop_all<1, 1, 1>(cctkGH, [&](const PointDesc &p) { kyy(p.I) = bt(p) / 2; });
  loop_all<1, 1, 1>(cctkGH, [&](const PointDesc &p) { kyz(p.I) = 0; });
  loop_all<1, 1, 1>(cctkGH, [&](const PointDesc &p) { kzz(p.I) = -bt(p) / 2; });
}

} // namespace CurvADM
