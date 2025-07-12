#ifndef CURVDERIVS_CURVTRANSDERIVS_HXX
#define CURVDERIVS_CURVTRANSDERIVS_HXX

#include <cx_powerinline.hxx>
#include <loop_device.hxx>

#include <array>
#include <cmath>

namespace CurvDerivs {
using namespace Loop;
using namespace CXUtils;
using namespace std;

template <int CI, int CJ, int CK, typename T>
CCTK_ATTRIBUTE_NOINLINE void
calc_transderivs(const GridDescBaseDevice &grid, const GF3D5layout &layout5,
                 const array<GF3D5<T>, 3> &tl_duO,
                 const array<GF3D5<T>, 6> &tl_dduO, const GF3D2layout &layout2,
                 const T *uI, const array<const T *, 9> &gf_Jac,
                 const array<const T *, 18> &gf_dJac) {

#include "../wolfram/transderivsinline.hxx"
}

template <int CI, int CJ, int CK, typename T>
CCTK_ATTRIBUTE_NOINLINE void
calc_transderivs(const GridDescBaseDevice &grid, const GF3D5layout &layout5,
                 const array<array<GF3D5<T>, 3>, 3> &tl_duO,
                 const array<array<GF3D5<T>, 6>, 3> &tl_dduO,
                 const GF3D2layout &layout2, const array<const T *, 3> &uI,
                 const array<const T *, 9> &gf_Jac,
                 const array<const T *, 18> &gf_dJac) {
  for (int a = 0; a < 3; ++a)
    calc_transderivs<CI, CJ, CK>(grid, layout5, tl_duO[a], tl_dduO[a], layout2,
                                 uI, gf_Jac, gf_dJac);
}

template <int CI, int CJ, int CK, typename T>
CCTK_ATTRIBUTE_NOINLINE void
calc_transderivs(const GridDescBaseDevice &grid, const GF3D5layout &layout5,
                 const array<array<GF3D5<T>, 3>, 6> &tl_duO,
                 const array<array<GF3D5<T>, 6>, 6> &tl_dduO,
                 const GF3D2layout &layout2, const array<const T *, 6> &uI,
                 const array<const T *, 9> &gf_Jac,
                 const array<const T *, 18> &gf_dJac) {
  for (int a = 0; a < 6; ++a)
    calc_transderivs<CI, CJ, CK>(grid, layout5, tl_duO[a], tl_dduO[a], layout2,
                                 uI, gf_Jac, gf_dJac);
}

} // namespace CurvDerivs

#endif // #ifndef CURVDERIVS_CURVTRANSDERIVS_HXX
