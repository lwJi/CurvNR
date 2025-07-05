#include <cctk.h>
#include <cctk_Arguments.h>
#include <cctk_Parameters.h>
#include <loop_device.hxx>

#include "CurvBase_MultiPatch.hxx"

namespace CurvBase {
using namespace Loop;

extern "C" int CurvBase_MultiPatch_Setup() {
  DECLARE_CCTK_PARAMETERS;

  ActiveMultiPatch tmp;

  if (CCTK_EQUALS(patch_system, "Cartesian")) {
    tmp.select_cartesian();
    tmp.get_mp1().add_patch(make_cart_patch());
  } else if (CCTK_EQUALS(patch_system, "Spherical")) {
    tmp.select_spherical();
    tmp.get_mp1().add_patch(make_sph_patch(spherical_rmin, spherical_rmax));
  } else if (CCTK_EQUALS(patch_system, "CubedSphere")) {
    tmp.select_cubedsphere();
    {
      auto &mp7 = tmp.get_mp7();
      const CCTK_REAL r0 = cubedsphere_rmin, r1 = cubedsphere_rmax;
      mp7.add_patch(make_cart_patch()); // core
      mp7.add_patch(make_wedge_patch(Face::PX, r0, r1));
      mp7.add_patch(make_wedge_patch(Face::NX, r0, r1));
      mp7.add_patch(make_wedge_patch(Face::PY, r0, r1));
      mp7.add_patch(make_wedge_patch(Face::NY, r0, r1));
      mp7.add_patch(make_wedge_patch(Face::PZ, r0, r1));
      mp7.add_patch(make_wedge_patch(Face::NZ, r0, r1));
    }
  } else {
    CCTK_VERROR("Unknown multi-patch system \"%s\"", patch_system);
  }

  active_mp() = tmp; // plain struct copy: variant knows which alt is active

  return 0;
}

extern "C" void CurvBase_MultiPatch_Coordinates_Setup(CCTK_ARGUMENTS) {
  DECLARE_CCTK_PARAMETERS;

  // access active multipatch system
  auto &mp = active_mp();
}

//------------------------------------------------------------------------------
// Interfaces with the CarpetX driver
//------------------------------------------------------------------------------

extern "C" CCTK_INT
CurvBase_MultiPatch_GetSystemSpecification(CCTK_INT *restrict const npatches) {
  *npatches = static_cast<CCTK_INT>(active_mp().size());
  return 0;
}

extern "C" CCTK_INT CurvBase_MultiPatch_GetPatchSpecification(
    const CCTK_INT ipatch, CCTK_INT *restrict const is_cartesian,
    const CCTK_INT size, CCTK_INT *restrict const ncells,
    CCTK_REAL *restrict const xmin, CCTK_REAL *restrict const xmax) {
  auto &mp = active_mp();

  assert(size == dim);
  assert(ipatch >= 0 && ipatch < static_cast<CCTK_INT>(mp.size()));

  const Patch *patch = mp.get_patch(static_cast<std::size_t>(ipatch));
  assert(patch != nullptr);

  if (is_cartesian) {
    *is_cartesian = patch->is_cartesian ? 1 : 0;
  }

  for (int d = 0; d < dim; ++d) {
    ncells[d] = patch->ncells[d];
    xmin[d] = patch->xmin[d];
    xmax[d] = patch->xmax[d];
  }

  return 0;
}

extern "C" CCTK_INT CurvBase_MultiPatch_GetBoundarySpecification(
    const CCTK_INT ipatch, const CCTK_INT size,
    CCTK_INT *restrict const is_interpatch_boundary) {
  auto &mp = active_mp();

  assert(size == 2 * dim);
  assert(ipatch >= 0 && ipatch < static_cast<CCTK_INT>(mp.size()));

  const Patch *patch = mp.get_patch(static_cast<std::size_t>(ipatch));
  assert(patch != nullptr);

  for (int d = 0; d < dim; ++d) {
    is_interpatch_boundary[2 * d + 0] =
        patch->faces[0][d].is_outer_boundary ? 0 : 1;
    is_interpatch_boundary[2 * d + 1] =
        patch->faces[1][d].is_outer_boundary ? 0 : 1;
  }

  return 0;
}

extern "C" void CurvBase_MultiPatch_GlobalToLocal(
    const CCTK_INT npoints, const CCTK_REAL *restrict const globalsx,
    const CCTK_REAL *restrict const globalsy,
    const CCTK_REAL *restrict const globalsz, CCTK_INT *restrict const patches,
    CCTK_REAL *restrict const localsx, CCTK_REAL *restrict const localsy,
    CCTK_REAL *restrict const localsz) {
  const auto &mp = active_mp();
  for (int n = 0; n < npoints; ++n) {
    const std::array<CCTK_REAL, dim> x{globalsx[n], globalsy[n], globalsz[n]};
    std::size_t patch_id = std::size_t(-1);
    const auto l = mp.g2l(x, patch_id);
    patches[n] = static_cast<CCTK_INT>(patch_id);
    localsx[n] = l[0];
    localsy[n] = l[1];
    localsz[n] = l[2];
  }
}

} // namespace CurvBase
