#include <loop_device.hxx>

#include <cctk.h>
#include <cctk_Arguments.h>
#include <cctk_Parameters.h>

#include <array>

namespace CurvTmunu {
using namespace std;
using namespace Loop;

extern "C" void CurvTmunu_ZeroTmunu(CCTK_ARGUMENTS) {
  DECLARE_CCTK_ARGUMENTSX_CurvTmunu_ZeroTmunu;
  DECLARE_CCTK_PARAMETERS;

  grid.loop_all_device<1, 1, 1>(grid.nghostzones,
                                [=] CCTK_DEVICE(const PointDesc &p)
                                    CCTK_ATTRIBUTE_ALWAYS_INLINE {
                                      eTtt(p.I) = 0;
                                      eTtx(p.I) = 0;
                                      eTty(p.I) = 0;
                                      eTtz(p.I) = 0;
                                      eTxx(p.I) = 0;
                                      eTxy(p.I) = 0;
                                      eTxz(p.I) = 0;
                                      eTyy(p.I) = 0;
                                      eTyz(p.I) = 0;
                                      eTzz(p.I) = 0;
                                    });
}

} // namespace CurvTmunu
