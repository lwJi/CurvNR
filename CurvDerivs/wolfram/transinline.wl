(* ::Package:: *)

(* transinline.wl *)

(* (c) Liwei Ji, 07/2025 *)

(******************)
(* Configurations *)
(******************)

Needs["xAct`xCoba`", FileNameJoin[{Environment["GENERATO"], "src/Generato.wl"}]]

SetPVerbose[False];

SetPrintDate[False];

SetPrintHeaderMacro[False];

SetGridPointIndex["[[ijk]]"];

SetTilePointIndex["[[ijk5]]"];

(*SetUseLetterForTensorComponet[True];*)

SetTempVariableType["auto"];

DefManifold[M3, 3, IndexRange[a, z]];

DefChart[cart, M3, {1, 2, 3}, {X[], Y[], Z[]}, ChartColor -> Blue];

(**********************************)
(* Define Variables and Equations *)
(**********************************)

JacVarlist =
  GridTensors[
    {Jac[k, -i], PrintAs -> "J"},
    {dJac[k, -i, -j], Symmetric[{-i, -j}], PrintAs -> "\[PartialD]J"}
  ];

DuInVarlist =
  TileTensors[
    {duI[-i], PrintAs -> "\[PartialD]\!\(\*SuperscriptBox[\(u\), \(in\)]\)"}
  ];

DDuInVarlist =
  TileTensors[
    {dduI[-i, -j], Symmetric[{-i, -j}],
     PrintAs -> "\[PartialD]\[PartialD]\!\(\*SuperscriptBox[\(u\), \(in\)]\)"}
  ];

DuOutVarlist =
  TileTensors[
    {duO[-i], PrintAs -> "\[PartialD]\!\(\*SuperscriptBox[\(u\), \(out\)]\)"}
  ];

DDuOutVarlist =
  TileTensors[
    {dduO[-i, -j], Symmetric[{-i, -j}],
     PrintAs -> "\[PartialD]\[PartialD]\!\(\*SuperscriptBox[\(u\), \(out\)]\)"}
  ];

(* Transformation Equations *)

SetEQN[duO[i_], Jac[k, i] duI[-k]];

SetEQN[dduO[i_, j_], dJac[k, i, j] duI[-k] + Jac[k, i] Jac[l, j] dduI[-k, -l]];

(******************)
(* Print to Files *)
(******************)

SetOutputFile[FileNameJoin[{Directory[], "transinline.hxx"}]];

SetMainPrint[
  (* Initialize grid function names *)
  PrintInitializations[{Mode -> "MainIn"}, JacVarlist];
  pr[];
  PrintInitializations[{Mode -> "Derivs", DerivsOrder -> 1,
                        StorageType -> "Tile", TensorType -> "Vect"},
                        DuOutVarlist];
  PrintInitializations[{Mode -> "Derivs", DerivsOrder -> 1,
                        StorageType -> "Tile", TensorType -> "Smat"},
                        DDuOutVarlist];
  PrintInitializations[{Mode -> "Derivs", DerivsOrder -> 1,
                        StorageType -> "Tile", TensorType -> "Vect"},
                        DuInVarlist];
  PrintInitializations[{Mode -> "Derivs", DerivsOrder -> 1,
                        StorageType -> "Tile", TensorType -> "Smat"},
                        DDuInVarlist];
  pr[];

  (* Loops *)
  pr["noinline([&]() __attribute__((__flatten__, __hot__)) {"];
  pr["grid.loop_int_device<CI, CJ, CK>("];
  pr["  grid.nghostzones, [=] ARITH_DEVICE(const PointDesc &p) ARITH_INLINE {"];
  pr["const int ijk = layout2.linear(p.i, p.j, p.k);"];
  pr["const int ijk5 = layout5.linear(p.i, p.j, p.k);"];
  pr[];

  PrintEquations[{Mode -> "Main"}, DuOutVarlist];
  PrintEquations[{Mode -> "Main"}, DDuOutVarlist];
  pr[];

  pr["});"];
  pr["});"];
];

Import[FileNameJoin[{Environment["GENERATO"], "codes/CarpetXGPU.wl"}]];
