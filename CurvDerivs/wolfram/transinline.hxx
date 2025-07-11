/* transinline.hxx */
/* Produced with Generato */

const auto Jac11 = gf_Jac[0];
const auto Jac12 = gf_Jac[1];
const auto Jac13 = gf_Jac[2];
const auto Jac21 = gf_Jac[3];
const auto Jac22 = gf_Jac[4];
const auto Jac23 = gf_Jac[5];
const auto Jac31 = gf_Jac[6];
const auto Jac32 = gf_Jac[7];
const auto Jac33 = gf_Jac[8];
const auto dJac111 = gf_dJac[0];
const auto dJac112 = gf_dJac[1];
const auto dJac113 = gf_dJac[2];
const auto dJac122 = gf_dJac[3];
const auto dJac123 = gf_dJac[4];
const auto dJac133 = gf_dJac[5];
const auto dJac211 = gf_dJac[6];
const auto dJac212 = gf_dJac[7];
const auto dJac213 = gf_dJac[8];
const auto dJac222 = gf_dJac[9];
const auto dJac223 = gf_dJac[10];
const auto dJac233 = gf_dJac[11];
const auto dJac311 = gf_dJac[12];
const auto dJac312 = gf_dJac[13];
const auto dJac313 = gf_dJac[14];
const auto dJac322 = gf_dJac[15];
const auto dJac323 = gf_dJac[16];
const auto dJac333 = gf_dJac[17];

const auto duO1 = tl_duO[0].ptr;
const auto duO2 = tl_duO[1].ptr;
const auto duO3 = tl_duO[2].ptr;
const auto dduO11 = tl_dduO[0].ptr;
const auto dduO12 = tl_dduO[1].ptr;
const auto dduO13 = tl_dduO[2].ptr;
const auto dduO22 = tl_dduO[3].ptr;
const auto dduO23 = tl_dduO[4].ptr;
const auto dduO33 = tl_dduO[5].ptr;
const auto duI1 = tl_duI[0].ptr;
const auto duI2 = tl_duI[1].ptr;
const auto duI3 = tl_duI[2].ptr;
const auto dduI11 = tl_dduI[0].ptr;
const auto dduI12 = tl_dduI[1].ptr;
const auto dduI13 = tl_dduI[2].ptr;
const auto dduI22 = tl_dduI[3].ptr;
const auto dduI23 = tl_dduI[4].ptr;
const auto dduI33 = tl_dduI[5].ptr;

noinline([&]() __attribute__((__flatten__, __hot__)) {
grid.loop_int_device<CI, CJ, CK>(
  grid.nghostzones, [=] ARITH_DEVICE(const PointDesc &p) ARITH_INLINE {
const int ijk = layout2.linear(p.i, p.j, p.k);
const int ijk5 = layout5.linear(p.i, p.j, p.k);

duO1[ijk5]
=
duI1[ijk5]*Jac11[ijk] + duI2[ijk5]*Jac21[ijk] + duI3[ijk5]*Jac31[ijk]
;

duO2[ijk5]
=
duI1[ijk5]*Jac12[ijk] + duI2[ijk5]*Jac22[ijk] + duI3[ijk5]*Jac32[ijk]
;

duO3[ijk5]
=
duI1[ijk5]*Jac13[ijk] + duI2[ijk5]*Jac23[ijk] + duI3[ijk5]*Jac33[ijk]
;

dduO11[ijk5]
=
dJac111[ijk]*duI1[ijk5] + dJac211[ijk]*duI2[ijk5] +
  dJac311[ijk]*duI3[ijk5] + dduI11[ijk5]*Power(Jac11[ijk],2) +
  2*dduI12[ijk5]*Jac11[ijk]*Jac21[ijk] + dduI22[ijk5]*Power(Jac21[ijk],2) +
  2*dduI13[ijk5]*Jac11[ijk]*Jac31[ijk] +
  2*dduI23[ijk5]*Jac21[ijk]*Jac31[ijk] + dduI33[ijk5]*Power(Jac31[ijk],2)
;

dduO12[ijk5]
=
dJac112[ijk]*duI1[ijk5] + dJac212[ijk]*duI2[ijk5] +
  dJac312[ijk]*duI3[ijk5] + dduI11[ijk5]*Jac11[ijk]*Jac12[ijk] +
  dduI12[ijk5]*Jac12[ijk]*Jac21[ijk] + dduI12[ijk5]*Jac11[ijk]*Jac22[ijk] +
  dduI22[ijk5]*Jac21[ijk]*Jac22[ijk] + dduI13[ijk5]*Jac12[ijk]*Jac31[ijk] +
  dduI23[ijk5]*Jac22[ijk]*Jac31[ijk] + dduI13[ijk5]*Jac11[ijk]*Jac32[ijk] +
  dduI23[ijk5]*Jac21[ijk]*Jac32[ijk] + dduI33[ijk5]*Jac31[ijk]*Jac32[ijk]
;

dduO13[ijk5]
=
dJac113[ijk]*duI1[ijk5] + dJac213[ijk]*duI2[ijk5] +
  dJac313[ijk]*duI3[ijk5] + dduI11[ijk5]*Jac11[ijk]*Jac13[ijk] +
  dduI12[ijk5]*Jac13[ijk]*Jac21[ijk] + dduI12[ijk5]*Jac11[ijk]*Jac23[ijk] +
  dduI22[ijk5]*Jac21[ijk]*Jac23[ijk] + dduI13[ijk5]*Jac13[ijk]*Jac31[ijk] +
  dduI23[ijk5]*Jac23[ijk]*Jac31[ijk] + dduI13[ijk5]*Jac11[ijk]*Jac33[ijk] +
  dduI23[ijk5]*Jac21[ijk]*Jac33[ijk] + dduI33[ijk5]*Jac31[ijk]*Jac33[ijk]
;

dduO22[ijk5]
=
dJac122[ijk]*duI1[ijk5] + dJac222[ijk]*duI2[ijk5] +
  dJac322[ijk]*duI3[ijk5] + dduI11[ijk5]*Power(Jac12[ijk],2) +
  2*dduI12[ijk5]*Jac12[ijk]*Jac22[ijk] + dduI22[ijk5]*Power(Jac22[ijk],2) +
  2*dduI13[ijk5]*Jac12[ijk]*Jac32[ijk] +
  2*dduI23[ijk5]*Jac22[ijk]*Jac32[ijk] + dduI33[ijk5]*Power(Jac32[ijk],2)
;

dduO23[ijk5]
=
dJac123[ijk]*duI1[ijk5] + dJac223[ijk]*duI2[ijk5] +
  dJac323[ijk]*duI3[ijk5] + dduI11[ijk5]*Jac12[ijk]*Jac13[ijk] +
  dduI12[ijk5]*Jac13[ijk]*Jac22[ijk] + dduI12[ijk5]*Jac12[ijk]*Jac23[ijk] +
  dduI22[ijk5]*Jac22[ijk]*Jac23[ijk] + dduI13[ijk5]*Jac13[ijk]*Jac32[ijk] +
  dduI23[ijk5]*Jac23[ijk]*Jac32[ijk] + dduI13[ijk5]*Jac12[ijk]*Jac33[ijk] +
  dduI23[ijk5]*Jac22[ijk]*Jac33[ijk] + dduI33[ijk5]*Jac32[ijk]*Jac33[ijk]
;

dduO33[ijk5]
=
dJac133[ijk]*duI1[ijk5] + dJac233[ijk]*duI2[ijk5] +
  dJac333[ijk]*duI3[ijk5] + dduI11[ijk5]*Power(Jac13[ijk],2) +
  2*dduI12[ijk5]*Jac13[ijk]*Jac23[ijk] + dduI22[ijk5]*Power(Jac23[ijk],2) +
  2*dduI13[ijk5]*Jac13[ijk]*Jac33[ijk] +
  2*dduI23[ijk5]*Jac23[ijk]*Jac33[ijk] + dduI33[ijk5]*Power(Jac33[ijk],2)
;


});
});

/* transinline.hxx */
