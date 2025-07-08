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

(* Coordinate Transformation Rules *)

Cart2SphRules = Thread[{x[], y[], z[]} -> CartInSph];
Sph2CartRules = Thread[{r[], \[Theta][], \[Phi][]} -> SphInCart];

(* Define Tensors using Components *)

JacSphInCartMat = Simplify@Outer[D, SphInCart, {x[], y[], z[]}];
JacCartInSphMat = Simplify@Outer[D, CartInSph, {r[], \[Theta][], \[Phi][]}];

JacCinS = CTensor[JacCartInSphMat, {cart, -sph}];
JacSinC = CTensor[JacSphInCartMat, {sph, -cart}];

(* Set Basis Transformation *)

SetBasisChange[JacSinC, cart]

(* WriteToC Rules *)
CartToCRules = {
    x[]^2 + y[]^2 + z[]^2 -> r^2,
    x[]^2 + y[]^2 -> rh^2,
    x[] -> x, y[] -> y, z[] -> z,
    r^(n_Integer?Negative) :> If[-n > 1, Symbol["rInv" <> ToString[-n]],
                                         Symbol["rInv"]],
    rh^(n_Integer?Negative) :> If[-n > 1, Symbol["rhInv" <> ToString[-n]],
                                          Symbol["rhInv"]]
};
SphToCRules = {
    r[] -> r,
    Sin[\[Theta][]] -> st, Cos[\[Theta][]] -> ct, Csc[\[Theta][]] -> stInv,
    Sin[\[Phi][]] -> sp, Cos[\[Phi][]] -> cp,
    r^(n_Integer?Negative) :> If[-n > 1, Symbol["rInv" <> ToString[-n]],
                                         Symbol["rInv"]],
    rh^(n_Integer?Negative) :> If[-n > 1, Symbol["rhInv" <> ToString[-n]],
                                          Symbol["rhInv"]],
    (* for dJac only *)
    Sin[2 \[Theta][]] -> s2t, Cos[2 \[Theta][]] -> c2t, Cot[\[Theta][]] -> cott,
    Sin[2 \[Phi][]] -> s2p, Cos[2 \[Phi][]] -> c2p
};

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

  (* Jacobian from Cartesian to Spherical in terms of Cartesian Coordinates *)

  pr["template <typename T>"];
  pr["CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr "
     <> "std::array<T, 9>"];
  pr["calc_jacSinC_inC(const std::array<T, 3> &xC) noexcept {"];
  pr["  const T x = xC[0], y = xC[1], z = xC[2];"];
  pr[];
  pr["  const T r2 = x*x + y*y + z*z;"];
  pr["  const T r = sqrt(r2);"];
  pr["  const T rh2 = x*x + y*y;"];
  pr["  const T rh = sqrt(rh2);"];
  pr[];
  pr["  const T rInv = T{1}/r;"];
  pr["  const T rInv2 = rInv*rInv;"];
  pr["  const T rhInv = T{1}/rh;"];
  pr["  const T rhInv2 = rhInv*rhInv;"];
  pr[];
  pr["  return {"];
  Do[
    If[ii != 3 || jj != 3,
      pr["    "
         <> ToString[CForm[
              Refine[
                JacSinC[{ii, sph}, {jj, -cart}] /. CartToCRules,
              {r > 0, rh > 0}] /. CartToCRules]]
         <> ","],
      pr["    "
         <> ToString[CForm[
              Refine[
                JacSinC[{ii, sph}, {jj, -cart}] /. CartToCRules,
              {r > 0, rh > 0}] /. CartToCRules]]]
    ],
  {ii, 1, 3}, {jj, 1, 3}];
  pr["  };"];
  pr["}"];
  pr[];

  (* Jacobian from Cartesian to Spherical in terms of Spherical Coordinates *)

  pr["template <typename T>"];
  pr["CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr "
     <>"std::array<T, 9>"];
  pr["calc_jacSinC_inS(const std::array<T, 3> &xS) noexcept {"];
  pr["  const T r = xS[0], th = xS[1], ph = xS[2];"];
  pr[];
  pr["  const T st = std::sin(th)"];
  pr["  const T ct = std::cos(th)"];
  pr["  const T sp = std::sin(ph)"];
  pr["  const T cp = std::cos(ph)"];
  pr[];
  pr["  const T rInv = T{1}/r;"];
  pr["  const T stInv = T{1}/st;"];
  pr[];
  pr["  return {"];
  Do[
    If[ii != 3 || jj != 3,
      pr["    "
         <> ToString[CForm[
              (JacSinC[{ii, sph}, {jj, -cart}] /. Cart2SphRules // FullSimplify)
                /. SphToCRules /. SphToCRules]] <> ","],
      pr["    "
         <> ToString[CForm[
              (JacSinC[{ii, sph}, {jj, -cart}] /. Cart2SphRules // FullSimplify)
               /. SphToCRules /. SphToCRules]]]
    ],
  {ii, 1, 3}, {jj, 1, 3}];
  pr["  };"];
  pr["}"];
  pr[];

  (* Jacobian derivatives from Cartesian to Spherical in terms of Cartesian
   * Coordinates *)

  pr["template <typename T>"];
  pr["CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr "
     <> "std::array<std::array<T, 9>, 3>"];
  pr["calc_djacSinC_inC(const std::array<T, 3> &xC) {"];
  pr["  const T x = xC[0], y = xC[1], z = xC[2];"];
  pr[];
  pr["  const T r2 = x*x + y*y + z*z;"];
  pr["  const T r = sqrt(r2);"];
  pr["  const T rh2 = x*x + y*y;"];
  pr["  const T rh = sqrt(rh2);"];
  pr[];
  pr["  const T rInv = T{1}/r;"];
  pr["  const T rInv2 = rInv*rInv;"];
  pr["  const T rInv3 = rInv2*rInv;"];
  pr["  const T rInv4 = rInv2*rInv2;"];
  pr["  const T rhInv = T{1}/rh;"];
  pr["  const T rhInv2 = rhInv*rhInv;"];
  pr["  const T rhInv3 = rhInv2*rhInv;"];
  pr[];
  pr["  return {"];
  Do[
    pr["    {"];
    Do[
      If[ii != 3 || jj != 3,
        pr["      "
           <> ToString[CForm[
                Refine[(PDcart[{kk, -cart}][JacSinC[{ii, sph}, {jj, -cart}]]
                  // Simplify) /. CartToCRules,
                {r > 0, rh > 0}] /. CartToCRules]] <> ","],
        pr["      "
           <> ToString[CForm[
                Refine[(PDcart[{kk, -cart}][JacSinC[{ii, sph}, {jj, -cart}]]
                  // Simplify) /. CartToCRules,
                {r > 0, rh > 0}] /. CartToCRules]]]
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

  (* Jacobian derivatives from Cartesian to Spherical in terms of Spherical
   * Coordinates *)

  pr["template <typename T>"];
  pr["CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr "
     <> "std::array<std::array<T, 9>, 3>"];
  pr["calc_djacSinC_inS(const std::array<T, 3> &xS) {"];
  pr["  const T r = xS[0], th = xS[1], ph = xS[2];"];
  pr["  const T st = std::sin(th)"];
  pr["  const T ct = std::cos(th)"];
  pr["  const T sp = std::sin(ph)"];
  pr["  const T cp = std::cos(ph)"];
  pr["  const T rInv = T{1}/r;"];
  pr["  const T stInv = T{1}/st;"];
  pr["  return {"];
  Do[
    pr["    {"];
    Do[
      If[ii != 3 || jj != 3,
        pr["      "
           <> ToString[CForm[
                (PDcart[{kk, -cart}][JacSinC[{ii, sph}, {jj, -cart}]]
                  /. Cart2SphRules // FullSimplify)
                /. SphToCRules /. SphToCRules]] <> ","],
        pr["      "
           <> ToString[CForm[
                (PDcart[{kk, -cart}][JacSinC[{ii, sph}, {jj, -cart}]]
                  /. Cart2SphRules // FullSimplify)
                /. SphToCRules /. SphToCRules]]]
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

  pr["} // namespace CurvBase"];
];

Import[FileNameJoin[{Environment["GENERATO"], "codes/CarpetXGPU.wl"}]];
