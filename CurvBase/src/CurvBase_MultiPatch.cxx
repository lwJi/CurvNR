#include <cctk.h>
#include <cctk_Arguments.h>
#include <cctk_Parameters.h>

#include "CurvBase_MultiPatch.hxx"

namespace CurvBase {
using namespace Loop;

extern "C" int CurvBase_MultiPatch_Setup() {
  DECLARE_CCTK_PARAMETERS;

  AMP tmp;

  if (CCTK_EQUALS(patch_system, "Cartesian")) {
    Index ncells{cartesian_ncells_i, cartesian_ncells_j, cartesian_ncells_k};
    Coord xmin{cartesian_xmin, cartesian_ymin, cartesian_zmin};
    Coord xmax{cartesian_xmax, cartesian_ymax, cartesian_zmax};
    tmp.select_cartesian(ncells, xmin, xmax);
  } else if (CCTK_EQUALS(patch_system, "Spherical")) {
    Index ncells{spherical_ncells_r, spherical_ncells_th, spherical_ncells_ph};
    Coord xmin{spherical_rmin, 0, 0};
    Coord xmax{spherical_rmax, onepi, twopi};
    std::array<bool, dim> cutouts{spherical_cutout_r != 0,
                                  spherical_cutout_th != 0,
                                  spherical_cutout_ph != 0};
    tmp.select_spherical(ncells, xmin, xmax, cutouts);
  } else if (CCTK_EQUALS(patch_system, "CubedSphere")) {
    Index ncells{cartesian_ncells_i, cartesian_ncells_j, cartesian_ncells_k};
    Coord xmin{-1.0, -1.0, -1.0};
    Coord xmax{1.0, 1.0, 1.0};
    tmp.select_cubedsphere(ncells, xmin, xmax, cubedsphere_rmin,
                           cubedsphere_rmax);
  } else {
    CCTK_VERROR("Unknown multi-patch system \"%s\"", patch_system);
  }

  active_mp() = tmp; // plain struct copy: variant knows which alt is active

  return 0;
}

extern "C" void CurvBase_MultiPatch_Coordinates_Setup(CCTK_ARGUMENTS) {
  DECLARE_CCTK_ARGUMENTSX_CurvBase_MultiPatch_Coordinates_Setup;
  DECLARE_CCTK_PARAMETERS;

  // access active multipatch system
  auto &mp = active_mp();

  grid.loop_all_device<0, 0, 0>(grid.nghostzones,
                                [=] ARITH_DEVICE(const Loop::PointDesc &p)
                                    ARITH_INLINE {
                                      const Coord l = {p.x, p.y, p.z};
                                      const Coord g = mp.l2g(p.patch, l);

                                      vcoordx(p.I) = g[0];
                                      vcoordy(p.I) = g[1];
                                      vcoordz(p.I) = g[2];
                                    });

  grid.loop_all_device<1, 1, 1>(grid.nghostzones,
                                [=] ARITH_DEVICE(const Loop::PointDesc &p)
                                    ARITH_INLINE {
                                      const Coord l = {p.x, p.y, p.z};
                                      const Coord g = mp.l2g(p.patch, l);

                                      ccoordx(p.I) = g[0];
                                      ccoordy(p.I) = g[1];
                                      ccoordz(p.I) = g[2];
                                    });
}

//==============================================================================
// Interfaces with the CarpetX driver
//==============================================================================

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

  if (is_cartesian != nullptr) {
    *is_cartesian = std::holds_alternative<CartesianMeta>(patch->meta) ? 1 : 0;
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
    const auto [l, patch_id] = mp.g2l(x);
    patches[n] = static_cast<CCTK_INT>(patch_id);
    localsx[n] = l[0];
    localsy[n] = l[1];
    localsz[n] = l[2];
  }
}

} // namespace CurvBase
