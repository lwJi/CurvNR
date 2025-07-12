/* derivs1st.hxx */
/* Produced with Generato */

const auto calcderivs1_1 =
    [=] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)
        CCTK_ATTRIBUTE_ALWAYS_INLINE {
switch (deriv_order) {
case 2: {
  return fd_1_o2<1>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 4: {
  return fd_1_o4<1>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 6: {
  return fd_1_o6<1>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 8: {
  return fd_1_o8<1>(layout2, gf_, i, j, k, invDxyz);
  break;
}
default: {
  assert(0);
  return 0.;
}
}
};

const auto calcderivs1_2 =
    [=] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)
        CCTK_ATTRIBUTE_ALWAYS_INLINE {
switch (deriv_order) {
case 2: {
  return fd_1_o2<2>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 4: {
  return fd_1_o4<2>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 6: {
  return fd_1_o6<2>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 8: {
  return fd_1_o8<2>(layout2, gf_, i, j, k, invDxyz);
  break;
}
default: {
  assert(0);
  return 0.;
}
}
};

const auto calcderivs1_3 =
    [=] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)
        CCTK_ATTRIBUTE_ALWAYS_INLINE {
switch (deriv_order) {
case 2: {
  return fd_1_o2<3>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 4: {
  return fd_1_o4<3>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 6: {
  return fd_1_o6<3>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 8: {
  return fd_1_o8<3>(layout2, gf_, i, j, k, invDxyz);
  break;
}
default: {
  assert(0);
  return 0.;
}
}
};


/* derivs1st.hxx */
