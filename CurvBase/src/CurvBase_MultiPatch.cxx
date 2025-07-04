
extern "C" void CurvBase_MultiPatch_Setup() {
  DECLARE_CCTK_PARAMETERS;

  ActiveMultiPatch tmp;

  switch (mode) {
  case MultiPatchMode::Cartesian:
    tmp.select_cartesian();
    tmp.get_mp1().add_patch(make_cart_patch());
    break;

  case MultiPatchMode::Spherical:
    tmp.select_spherical();
    tmp.get_mp1().add_patch(make_sph_patch(spherical_rmin, spherical_rmax));
    break;

  case MultiPatchMode::CubedSphere:
    tmp.select_cubedsphere();
    {
      auto &mp7 = tmp.get_mp7();
      const CCTK_REAL r0 = cubedsphere_rmin, r1 = cubedsphere_rmax;
      mp7.add_patch(make_wedge(Face::PX, r0, r1));
      mp7.add_patch(make_wedge(Face::NX, r0, r1));
      mp7.add_patch(make_wedge(Face::PY, r0, r1));
      mp7.add_patch(make_wedge(Face::NY, r0, r1));
      mp7.add_patch(make_wedge(Face::PZ, r0, r1));
      mp7.add_patch(make_wedge(Face::NZ, r0, r1));
      mp7.add_patch(make_cart_patch()); // core
    }
    break;

  default:
    CCTK_VERROR("Unknown multi-patch system \"%s\"", patch_system);
  }

  active_mp() = tmp; // plain struct copy: variant knows which alt is active
}

extern "C" void CurvBase_MultiPatch_Setup_Coordinates() {
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
