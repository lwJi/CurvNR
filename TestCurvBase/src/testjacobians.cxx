#include <loop_device.hxx>

#include <CurvBase_MultiPatch.hxx>

#include <cctk.h>
#include <cctk_Arguments.h>
#include <cctk_Parameters.h>

namespace TestCurvBase {
using namespace Loop;
using namespace CurvBase;
using namespace std;

extern "C" void TestCurvBase_CalcJacobianErrors(CCTK_ARGUMENTS) {
  DECLARE_CCTK_ARGUMENTSX_TestCurvBase_CalcJacobianErrors;
  DECLARE_CCTK_PARAMETERS;

  auto mp = active_mp();

  if (CCTK_EQUALS(patch_system, "Spherical")) {

    grid.loop_all_device<1, 1, 1>(
        grid.nghostzones,
        [=] CCTK_DEVICE(const Loop::PointDesc &p) CCTK_ATTRIBUTE_ALWAYS_INLINE {
          const Coord l = {p.x, p.y, p.z};
          const Coord g = {ccoordx(p.I), ccoordy(p.I), ccoordz(p.I)};

          const Jac_t jac = mp->jac_g2l_g(p.patch, g);
          const dJac_t djac = mp->djac_g2l_g(p.patch, g);

          err_cJ1x(p.I) = jac[0][0] - cJ1x(p.I);
          err_cJ1y(p.I) = jac[0][1] - cJ1y(p.I);
          err_cJ1z(p.I) = jac[0][2] - cJ1z(p.I);
          err_cJ2x(p.I) = jac[1][0] - cJ2x(p.I);
          err_cJ2y(p.I) = jac[1][1] - cJ2y(p.I);
          err_cJ2z(p.I) = jac[1][2] - cJ2z(p.I);
          err_cJ3x(p.I) = jac[2][0] - cJ3x(p.I);
          err_cJ3y(p.I) = jac[2][1] - cJ3y(p.I);
          err_cJ3z(p.I) = jac[2][2] - cJ3z(p.I);

          err_cdJ1xx(p.I) = djac[0][0] - cdJ1xx(p.I);
          err_cdJ1xy(p.I) = djac[0][1] - cdJ1xy(p.I);
          err_cdJ1xz(p.I) = djac[0][2] - cdJ1xz(p.I);
          err_cdJ1yy(p.I) = djac[0][3] - cdJ1yy(p.I);
          err_cdJ1yz(p.I) = djac[0][4] - cdJ1yz(p.I);
          err_cdJ1zz(p.I) = djac[0][5] - cdJ1zz(p.I);
          err_cdJ2xx(p.I) = djac[1][0] - cdJ2xx(p.I);
          err_cdJ2xy(p.I) = djac[1][1] - cdJ2xy(p.I);
          err_cdJ2xz(p.I) = djac[1][2] - cdJ2xz(p.I);
          err_cdJ2yy(p.I) = djac[1][3] - cdJ2yy(p.I);
          err_cdJ2yz(p.I) = djac[1][4] - cdJ2yz(p.I);
          err_cdJ2zz(p.I) = djac[1][5] - cdJ2zz(p.I);
          err_cdJ3xx(p.I) = djac[2][0] - cdJ3xx(p.I);
          err_cdJ3xy(p.I) = djac[2][1] - cdJ3xy(p.I);
          err_cdJ3xz(p.I) = djac[2][2] - cdJ3xz(p.I);
          err_cdJ3yy(p.I) = djac[2][3] - cdJ3yy(p.I);
          err_cdJ3yz(p.I) = djac[2][4] - cdJ3yz(p.I);
          err_cdJ3zz(p.I) = djac[2][5] - cdJ3zz(p.I);
        });

  } else {
    CCTK_ERROR("Unknown patch system");
  }
}

} // namespace TestCurvBase
