
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
}
