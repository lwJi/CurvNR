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
    pr["    [layout2, invDxyz] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)"];
    pr["        CCTK_ATTRIBUTE_ALWAYS_INLINE {"];
    Do[
      If[aOrd == 2,
        pr["if constexpr (DORDER == " <> ToString[aOrd] <> ") {"],
        pr["} else if constexpr (DORDER == " <> ToString[aOrd] <> ") {"]
      ];
      pr["  return fd_1_o" <> ToString[aOrd] <> "<" <> ToString[iDir]
                           <> ">(layout2, gf_, i, j, k, invDxyz);"];
      ,
      {aOrd, 2, 8, 2}
    ];
    pr["} else {"];
    pr["  static_assert(false, \"Unsupported derivative order\");"];
    pr["  return 0.;"];
    pr["}"];
    pr["};"];
    pr[]
    ,
    {iDir, 1, 3}
  ];
];

Import[FileNameJoin[{Environment["GENERATO"], "codes/CarpetXGPU.wl"}]];
