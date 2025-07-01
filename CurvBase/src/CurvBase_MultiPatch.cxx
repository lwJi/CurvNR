
extern "C" void CurvBase_MultiPatch_Setup() {
  DECLARE_CCTK_PARAMETERS;

  {
    ActiveMultiPatch tmp;
    tmp.mode = mode;

    if (mode == MultiPatchMode::Cartesian) {
      tmp.mp1.add_patch(make_cart_patch());
    } else if (mode == MultiPatchMode::Spherical) {
      tmp.mp1.add_patch(make_sph_patch(p.r_min, p.r_max));
    } else if (mode == MultiPatchMode::CubeSphere) {
      // CubeSphere: 6 wedges + central Cartesian at the end
      tmp.mp7.add_patch(make_wedge(Face::PX, p.r_min, p.r_max)); // id 0
      tmp.mp7.add_patch(make_wedge(Face::NX, p.r_min, p.r_max)); // id 1
      tmp.mp7.add_patch(make_wedge(Face::PY, p.r_min, p.r_max)); // id 2
      tmp.mp7.add_patch(make_wedge(Face::NY, p.r_min, p.r_max)); // id 3
      tmp.mp7.add_patch(make_wedge(Face::PZ, p.r_min, p.r_max)); // id 4
      tmp.mp7.add_patch(make_wedge(Face::NZ, p.r_min, p.r_max)); // id 5
      tmp.mp7.add_patch(make_cart_patch());                      // id 6 (core)
    } else {
      CCTK_VERROR("Unknown patch system \"%s\"", patch_system);
    }

    active_mp() = tmp; // move into the global store
  }
}

extern "C" void CurvBase_MultiPatch_Setup_Coordinates() {
  DECLARE_CCTK_PARAMETERS;

  // access active multipatch system
  auto &mp = active_mp();
}

// Interfaces with the CarpetX driver

extern "C" CCTK_INT
CurvBase_MultiPatch_GetSystemSpecification(CCTK_INT *restrict const npatches) {
  auto &mp = active_mp();
  *npatches = mp->num_patches();
  return 0;
}

extern "C" CCTK_INT CurvBase_MultiPatch_GetPatchSpecification(
    const CCTK_INT ipatch, CCTK_INT *restrict const is_cartesian,
    const CCTK_INT size, CCTK_INT *restrict const ncells,
    CCTK_REAL *restrict const xmin, CCTK_REAL *restrict const xmax) {
  auto &mp = active_mp();
  assert(ipatch >= 0 && ipatch < mp->num_patches());
  assert(size == dim);
  const Patch &patch = mp->patches.at(ipatch);

  if (is_cartesian != nullptr) {
    *is_cartesian = static_cast<CCTK_INT>(patch.is_cartesian);
  }

  for (int d = 0; d < dim; ++d) {
    ncells[d] = patch.ncells[d];
    xmin[d] = patch.xmin[d];
    xmax[d] = patch.xmax[d];
  }
  return 0;
}

extern "C" CCTK_INT CurvBase_MultiPatch_GetBoundarySpecification(
    const CCTK_INT ipatch, const CCTK_INT size,
    CCTK_INT *restrict const is_interpatch_boundary) {
  auto &mp = active_mp();
  assert(ipatch >= 0 && ipatch < mp->num_patches());
  assert(size == 2 * dim);
  const Patch &patch = mp->patches.at(ipatch);
  for (int f = 0; f < 2; ++f)
    for (int d = 0; d < dim; ++d)
      is_interpatch_boundary[2 * d + f] = !patch.faces[f][d].is_outer_boundary;
  return 0;
}
