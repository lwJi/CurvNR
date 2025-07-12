/* derivs2nd.hxx */
/* Produced with Generato */

const auto calcderivs2_11 =
    [=] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)
        CCTK_ATTRIBUTE_ALWAYS_INLINE {
switch (deriv_order) {
case 2: {
  return fd_2_o2<1, 1>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 4: {
  return fd_2_o4<1, 1>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 6: {
  return fd_2_o6<1, 1>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 8: {
  return fd_2_o8<1, 1>(layout2, gf_, i, j, k, invDxyz);
  break;
}
default: {
  assert(0);
  return 0.;
}
}
};

const auto calcderivs2_12 =
    [=] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)
        CCTK_ATTRIBUTE_ALWAYS_INLINE {
switch (deriv_order) {
case 2: {
  return fd_2_o2<1, 2>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 4: {
  return fd_2_o4<1, 2>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 6: {
  return fd_2_o6<1, 2>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 8: {
  return fd_2_o8<1, 2>(layout2, gf_, i, j, k, invDxyz);
  break;
}
default: {
  assert(0);
  return 0.;
}
}
};

const auto calcderivs2_13 =
    [=] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)
        CCTK_ATTRIBUTE_ALWAYS_INLINE {
switch (deriv_order) {
case 2: {
  return fd_2_o2<1, 3>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 4: {
  return fd_2_o4<1, 3>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 6: {
  return fd_2_o6<1, 3>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 8: {
  return fd_2_o8<1, 3>(layout2, gf_, i, j, k, invDxyz);
  break;
}
default: {
  assert(0);
  return 0.;
}
}
};

const auto calcderivs2_22 =
    [=] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)
        CCTK_ATTRIBUTE_ALWAYS_INLINE {
switch (deriv_order) {
case 2: {
  return fd_2_o2<2, 2>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 4: {
  return fd_2_o4<2, 2>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 6: {
  return fd_2_o6<2, 2>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 8: {
  return fd_2_o8<2, 2>(layout2, gf_, i, j, k, invDxyz);
  break;
}
default: {
  assert(0);
  return 0.;
}
}
};

const auto calcderivs2_23 =
    [=] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)
        CCTK_ATTRIBUTE_ALWAYS_INLINE {
switch (deriv_order) {
case 2: {
  return fd_2_o2<2, 3>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 4: {
  return fd_2_o4<2, 3>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 6: {
  return fd_2_o6<2, 3>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 8: {
  return fd_2_o8<2, 3>(layout2, gf_, i, j, k, invDxyz);
  break;
}
default: {
  assert(0);
  return 0.;
}
}
};

const auto calcderivs2_33 =
    [=] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)
        CCTK_ATTRIBUTE_ALWAYS_INLINE {
switch (deriv_order) {
case 2: {
  return fd_2_o2<3, 3>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 4: {
  return fd_2_o4<3, 3>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 6: {
  return fd_2_o6<3, 3>(layout2, gf_, i, j, k, invDxyz);
  break;
}
case 8: {
  return fd_2_o8<3, 3>(layout2, gf_, i, j, k, invDxyz);
  break;
}
default: {
  assert(0);
  return 0.;
}
}
};


/* derivs2nd.hxx */
