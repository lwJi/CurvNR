(* ::Package:: *)

(* JacobianCart2Sph.wl *)

(* (c) Liwei Ji, 02/2025 *)

(******************)
(* Configurations *)
(******************)

Needs["xAct`xCoba`", FileNameJoin[{Environment["GENERATO"], "src/Generato.wl"}]]

SetPVerbose[False];

SetPrintDate[False];

(**********)
(* Set Up *)
(**********)

DefManifold[R3, 3, {a, b, c, d, e, f, i, j, k}];

DefChart[sph, R3, {1, 2, 3}, {r[], \[Theta][], \[Phi][]}, ChartColor -> Red];
DefChart[cart, R3, {1, 2, 3}, {x[], y[], z[]}, ChartColor -> Green];
	
$Assumptions = And[
  (x[] | y[] | z[] | r[] | \[Theta][] | \[Phi][]) \[Element] Reals,
  r[] > 0,
  0 < \[Theta][] < Pi,
  0 <= \[Phi][] < 2 Pi
];

(* Coordinate Transformation *)

SphInCart = {
  Sqrt[x[]^2 + y[]^2 + z[]^2],
  ArcCos[z[]/Sqrt[x[]^2 + y[]^2 + z[]^2]],
  ArcTan[x[], y[]]
};

CartInSph = r[] {
  Sin[\[Theta][]] Cos[\[Phi][]],
  Sin[\[Theta][]] Sin[\[Phi][]],
  Cos[\[Theta][]]
};

(* Define Tensors using Components *)

JacSphInCartMat = Simplify@Outer[D, SphInCart, {x[], y[], z[]}];
JacCartInSphMat = Simplify@Outer[D, CartInSph, {r[], \[Theta][], \[Phi][]}];

JacCinS = CTensor[JacCartInSphMat, {cart, -sph}];
JacSinC = CTensor[JacSphInCartMat, {sph, -cart}];

(* Set Basis Transformation *)

SetBasisChange[JacSinC, cart]

(* Set Components for Coordinates *)
ComponentValue[x[], x];
ComponentValue[y[], y];
ComponentValue[z[], z];

(******************)
(* Print to Files *)
(******************)

SetOutputFile[FileNameJoin[{Directory[], "JacobianCart2Sph.hxx"}]];

SetMainPrint[
  pr["#include <loop_device.hxx>"];
  pr[];
  pr["#include <array>"];
  pr["#include <cmath>"];
  pr[];
  pr["#include \"powerinline.hxx\""];
  pr[];

  pr["namespace CurvBase {"];
  pr["using namespace Loop;"];
  pr[];

  pr["template <typename T>"];
  pr["CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE inline constexpr std::array<T, 9>"];
  pr["calc_jacSinC_inC(const std::array<T, 3> &xC) noexcept {"];
  pr["  const T &x = xC[0], &y = xC[1], &z = xC[2];"];
  pr["  return {"];
  Do[
    If[ii != 3 || jj != 3,
      pr["    " <> ToString[CForm[JacSinC[{ii, sph}, {jj, -cart}] // ToValues]] <> ","],
      pr["    " <> ToString[CForm[JacSinC[{ii, sph}, {jj, -cart}] // ToValues]]]
    ],
  {ii, 1, 3}, {jj, 1, 3}];
  pr["  };"];
  pr["}"];
  pr[];

  pr["template <typename T>"];
  pr["CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE inline constexpr std::array<T, 9>"];
  pr["calc_jacSinC_inS(const std::array<T, 3> &xS) noexcept {"];
  pr["  const T &r = xS[0], &th = xS[1], &ph = xS[2];"];
  pr["  const T sth = std::sin(th)"];
  pr["  const T cth = std::cos(th)"];
  pr["  const T sph = std::sin(ph)"];
  pr["  const T cph = std::cos(ph)"];
  pr["  return {"];
  Do[
    If[ii != 3 || jj != 3,
      pr["    " <> ToString[CForm[JacSinC[{ii, sph}, {jj, -cart}] // ToValues]] <> ","],
      pr["    " <> ToString[CForm[JacSinC[{ii, sph}, {jj, -cart}] // ToValues]]]
    ],
  {ii, 1, 3}, {jj, 1, 3}];
  pr["  };"];
  pr["}"];
  pr[];

  (*
  pr["template <typename T>"];
  pr["CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE inline std::array<std::array<T, 9>, 3>"];
  pr["calc_djacSinC(const std::array<T, 3> &xC) {"];
  pr["  const T &x = xC[0], &y = xC[1], &z = xC[2];"];
  pr["  return {"];
  Do[
    pr["    {"];
    Do[
      If[ii != 3 || jj != 3,
        pr["      " <> ToString[CForm[PDcart[{kk, -cart}][JacSinC[{ii, sph}, {jj, -cart}]] // ToValues]] <> ","],
        pr["      " <> ToString[CForm[PDcart[{kk, -cart}][JacSinC[{ii, sph}, {jj, -cart}]] // ToValues]]]
      ],
    {ii, 1, 3}, {jj, 1, 3}];
    If[kk != 3,
      pr["    },"],
      pr["    }"]
    ],
  {kk, 1, 3}];
  pr["  };"];
  pr["}"];
  pr[];
  *)

  pr["} // namespace CurvBase"];
];

Import[FileNameJoin[{Environment["GENERATO"], "codes/CarpetXGPU.wl"}]];
