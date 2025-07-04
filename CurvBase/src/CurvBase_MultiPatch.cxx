
extern "C" void CurvBase_MultiPatch_Setup() {
  DECLARE_CCTK_PARAMETERS;

  ActiveMultiPatch tmp;
  tmp.mode = mode;

  switch (mode) {
  case MultiPatchMode::Cartesian:
    tmp.mp1.add_patch(make_cart_patch());
    break;

  case MultiPatchMode::Spherical:
    tmp.mp1.add_patch(make_sph_patch(p.r_min, p.r_max));
    break;

  case MultiPatchMode::CubedSphere:
    // CubedSphere: 6 wedges + central Cartesian at the end
    tmp.mp7.add_patch(make_wedge(Face::PX, p.r_min, p.r_max)); // id 0
    tmp.mp7.add_patch(make_wedge(Face::NX, p.r_min, p.r_max)); // id 1
    tmp.mp7.add_patch(make_wedge(Face::PY, p.r_min, p.r_max)); // id 2
    tmp.mp7.add_patch(make_wedge(Face::NY, p.r_min, p.r_max)); // id 3
    tmp.mp7.add_patch(make_wedge(Face::PZ, p.r_min, p.r_max)); // id 4
    tmp.mp7.add_patch(make_wedge(Face::NZ, p.r_min, p.r_max)); // id 5
    tmp.mp7.add_patch(make_cart_patch());                      // id 6 (core)
    break;

  default:
    CCTK_VERROR("Unknown multi-patch system \"%s\"", patch_system);
  }

  active_mp() = tmp; // move into the global store
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
