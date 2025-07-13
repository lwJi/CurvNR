/* derivs1st.hxx */
/* Produced with Generato */

const auto calcderivs1_1 =
    [layout2, invDxyz] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)
        CCTK_ATTRIBUTE_ALWAYS_INLINE {
if constexpr (DORDER == 2) {
  return fd_1_o2<1>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 4) {
  return fd_1_o4<1>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 6) {
  return fd_1_o6<1>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 8) {
  return fd_1_o8<1>(layout2, gf_, i, j, k, invDxyz);
} else {
  static_assert(false, "Unsupported derivative order");
  return 0.;
}
};

const auto calcderivs1_2 =
    [layout2, invDxyz] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)
        CCTK_ATTRIBUTE_ALWAYS_INLINE {
if constexpr (DORDER == 2) {
  return fd_1_o2<2>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 4) {
  return fd_1_o4<2>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 6) {
  return fd_1_o6<2>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 8) {
  return fd_1_o8<2>(layout2, gf_, i, j, k, invDxyz);
} else {
  static_assert(false, "Unsupported derivative order");
  return 0.;
}
};

const auto calcderivs1_3 =
    [layout2, invDxyz] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)
        CCTK_ATTRIBUTE_ALWAYS_INLINE {
if constexpr (DORDER == 2) {
  return fd_1_o2<3>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 4) {
  return fd_1_o4<3>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 6) {
  return fd_1_o6<3>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 8) {
  return fd_1_o8<3>(layout2, gf_, i, j, k, invDxyz);
} else {
  static_assert(false, "Unsupported derivative order");
  return 0.;
}
};


/* derivs1st.hxx */
