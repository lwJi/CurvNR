/* derivs2nd.hxx */
/* Produced with Generato */

const auto calcderivs2_11 =
    [layout2, invDxyz] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)
        CCTK_ATTRIBUTE_ALWAYS_INLINE {
if constexpr (DORDER == 2) {
  return fd_2_o2<1, 1>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 4) {
  return fd_2_o4<1, 1>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 6) {
  return fd_2_o6<1, 1>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 8) {
  return fd_2_o8<1, 1>(layout2, gf_, i, j, k, invDxyz);
} else {
  static_assert(DORDER <= 8, "Unsupported derivative order");
  return 0.;
}
};

const auto calcderivs2_12 =
    [layout2, invDxyz] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)
        CCTK_ATTRIBUTE_ALWAYS_INLINE {
if constexpr (DORDER == 2) {
  return fd_2_o2<1, 2>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 4) {
  return fd_2_o4<1, 2>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 6) {
  return fd_2_o6<1, 2>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 8) {
  return fd_2_o8<1, 2>(layout2, gf_, i, j, k, invDxyz);
} else {
  static_assert(DORDER <= 8, "Unsupported derivative order");
  return 0.;
}
};

const auto calcderivs2_13 =
    [layout2, invDxyz] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)
        CCTK_ATTRIBUTE_ALWAYS_INLINE {
if constexpr (DORDER == 2) {
  return fd_2_o2<1, 3>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 4) {
  return fd_2_o4<1, 3>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 6) {
  return fd_2_o6<1, 3>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 8) {
  return fd_2_o8<1, 3>(layout2, gf_, i, j, k, invDxyz);
} else {
  static_assert(DORDER <= 8, "Unsupported derivative order");
  return 0.;
}
};

const auto calcderivs2_22 =
    [layout2, invDxyz] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)
        CCTK_ATTRIBUTE_ALWAYS_INLINE {
if constexpr (DORDER == 2) {
  return fd_2_o2<2, 2>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 4) {
  return fd_2_o4<2, 2>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 6) {
  return fd_2_o6<2, 2>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 8) {
  return fd_2_o8<2, 2>(layout2, gf_, i, j, k, invDxyz);
} else {
  static_assert(DORDER <= 8, "Unsupported derivative order");
  return 0.;
}
};

const auto calcderivs2_23 =
    [layout2, invDxyz] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)
        CCTK_ATTRIBUTE_ALWAYS_INLINE {
if constexpr (DORDER == 2) {
  return fd_2_o2<2, 3>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 4) {
  return fd_2_o4<2, 3>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 6) {
  return fd_2_o6<2, 3>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 8) {
  return fd_2_o8<2, 3>(layout2, gf_, i, j, k, invDxyz);
} else {
  static_assert(DORDER <= 8, "Unsupported derivative order");
  return 0.;
}
};

const auto calcderivs2_33 =
    [layout2, invDxyz] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)
        CCTK_ATTRIBUTE_ALWAYS_INLINE {
if constexpr (DORDER == 2) {
  return fd_2_o2<3, 3>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 4) {
  return fd_2_o4<3, 3>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 6) {
  return fd_2_o6<3, 3>(layout2, gf_, i, j, k, invDxyz);
} else if constexpr (DORDER == 8) {
  return fd_2_o8<3, 3>(layout2, gf_, i, j, k, invDxyz);
} else {
  static_assert(DORDER <= 8, "Unsupported derivative order");
  return 0.;
}
};


/* derivs2nd.hxx */
