(* ::Package:: *)

(* derivs2nd.wl *)

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

SetOutputFile[FileNameJoin[{Directory[], "derivs2nd.hxx"}]];

SetMainPrint[
  Do[
    pr["const auto calcderivs2_" <> ToString[iDir1] <> ToString[iDir2] <> " ="];
    pr["    [layout2, invDxyz] CCTK_DEVICE(const CCTK_REAL *gf_, int i, int j, int k)"];
    pr["        CCTK_ATTRIBUTE_ALWAYS_INLINE {"];
    Do[
      If[aOrd == 2,
        pr["if constexpr (DORDER == " <> ToString[aOrd] <> ") {"],
        pr["} else if constexpr (DORDER == " <> ToString[aOrd] <> ") {"]
      ];
      pr["  return fd_2_o" <> ToString[aOrd] <> "<"
                           <> ToString[iDir1] <> ", " <> ToString[iDir2]
                           <> ">(layout2, gf_, i, j, k, invDxyz);"];
      ,
      {aOrd, 2, 8, 2}
    ];
    pr["} else {"];
    pr["  static_assert(DORDER <= 8, \"Unsupported derivative order\");"];
    pr["  return 0.;"];
    pr["}"];
    pr["};"];
    pr[]
    ,
    {iDir1, 1, 3}, {iDir2, iDir1, 3}
  ];
];

Import[FileNameJoin[{Environment["GENERATO"], "codes/CarpetXGPU.wl"}]];
