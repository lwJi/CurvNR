(* ::Package:: *)

(* derivs1st.wl *)

(* (c) Liwei Ji, 02/2025 *)

(******************)
(* Configurations *)
(******************)

Needs["xAct`xCoba`", FileNameJoin[{Environment["GENERATO"], "src/Generato.wl"}]]

SetPVerbose[False];

SetPrintDate[False];

SetPrintHeaderMacro[False];


(******************)
(* Print to Files *)
(******************)

SetOutputFile[FileNameJoin[{Directory[], "derivs1st.hxx"}]];

SetMainPrint[
  Do[
    pr["const auto calcderivs1_" <> ToString[iDir] <> " ="];
    pr["    [=] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)"];
    pr["        CCTK_ATTRIBUTE_ALWAYS_INLINE {"];
    pr["switch (deriv_order) {"];
    Do[
      pr["case " <> ToString[aOrd] <> ": {"];
      pr["  return fd_1_o" <> ToString[aOrd] <> "<" <> ToString[iDir]
                           <> ">(layout2, gf_, i, j, k, invDxyz);"];
      pr["  break;"];
      pr["}"]
      ,
      {aOrd, 2, 8, 2}
    ];
    pr["default: {"];
    pr["  assert(0);"];
    pr["  return 0.;"];
    pr["}"];
    pr["}"];
    pr["};"];
    pr[]
    ,
    {iDir, 1, 3}
  ];
];

Import[FileNameJoin[{Environment["GENERATO"], "codes/CarpetXGPU.wl"}]];
