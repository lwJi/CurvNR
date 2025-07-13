(* ::Package:: *)

(* JacobianCart2Cyl.wl *)

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

DefChart[cyl, R3, {1, 2, 3}, {rh[], \[Phi][], z[]}, ChartColor -> Red];
DefChart[cart, R3, {1, 2, 3}, {x[], y[], z[]}, ChartColor -> Green];
	
$Assumptions = And[
  (x[] | y[] | z[] | rh[] | \[Phi][]) \[Element] Reals,
  rh[] > 0,
  0 <= \[Phi][] < 2 Pi
];

(* Coordinate Transformation *)

CylInCart = {
  Sqrt[x[]^2 + y[]^2],
  ArcTan[x[], y[]],
  z[]
};

CartInCyl = {
  rh[] Cos[\[Phi][]],
  rh[] Sin[\[Phi][]],
  z[]
};

(* Coordinate Transformation Rules *)

Cart2CylRules = Thread[{x[], y[], z[]} -> CartInCyl];
Cyl2CartRules = Thread[{rh[], \[Phi][], z[]} -> CylInCart];

(* Define Tensors using Components *)

JacCylInCartMat = Simplify@Outer[D, CylInCart, {x[], y[], z[]}];
JacCartInCylMat = Simplify@Outer[D, CartInCyl, {rh[], \[Phi][], z[]}];

JacCinCyl = CTensor[JacCartInCylMat, {cart, -cyl}];
JacCylinC = CTensor[JacCylInCartMat, {cyl, -cart}];

(* Set Basis Transformation *)

SetBasisChange[JacCylinC, cart]

(* WriteToC Rules *)

CartToCRules = {
  x[]^2 + y[]^2 -> rh^2,
  x[] -> x, y[] -> y, z[] -> z
};

CylToCRules = {
  rh[] -> rh,
  Sin[\[Phi][]] -> sp, Cos[\[Phi][]] -> cp,
  Sin[2 \[Phi][]] -> s2p, Cos[2 \[Phi][]] -> c2p
};

PowerToCRules = Module[{validSymbols = {rh, x, y, z, sp, cp}},
  {
    base_Symbol^(n_Integer) /; MemberQ[validSymbols, base] :>
      If[n > 0,
          Symbol[SymbolName[base] <> ToString[n]],
          If[n == -1,
            Symbol[SymbolName[base] <> "Inv"],
            Symbol[SymbolName[base] <> "Inv" <> ToString[-n]]]]
  }
];

(******************)
(* Print to Files *)
(******************)

SetOutputFile[FileNameJoin[{Directory[], "JacobianCart2Cyl.hxx"}]];

SetMainPrint[
  pr["#include <loop_device.hxx>"];
  pr[];
  pr["#include <array>"];
  pr["#include <cmath>"];
  pr[];

  pr["namespace CurvBase {"];
  pr["using namespace Loop;"];
  pr[];

  (* Jacobian from Cartesian to Cylerical in terms of Cartesian Coordinates *)

  pr["template <typename T>"];
  pr["CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr "
     <> "std::array<std::array<T, 3>, 3>"];
  pr["jac_cart2cyl_cart(const std::array<T, 3> &xC) noexcept {"];
  pr["  const T x = xC[0], y = xC[1], z = xC[2];"];
  pr[];
  pr["  const T x2 = x*x;"];
  pr["  const T y2 = y*y;"];
  pr[];
  pr["  const T rh2 = x2 + y2;"];
  pr["  const T rh = sqrt(rh2);"];
  pr[];
  pr["  const T rhInv = T{1}/rh;"];
  pr["  const T rhInv2 = rhInv*rhInv;"];
  pr[];
  pr["  return {{"];
  Do[
    pr["    {"];
    Do[
      If[ii != 3,
        pr["      "
           <> ToString[CForm[
                Refine[
                  JacCylinC[{kk, cyl}, {ii, -cart}] /. CartToCRules,
                {r > 0, rh > 0}] /. PowerToCRules]]
           <> ","],
        pr["      "
           <> ToString[CForm[
                Refine[
                  JacCylinC[{kk, cyl}, {ii, -cart}] /. CartToCRules,
                {r > 0, rh > 0}] /. PowerToCRules]]]
      ],
    {ii, 1, 3}];
    If[kk != 3, pr["    },"], pr["    }"]],
  {kk, 1, 3}];
  pr["  }};"];
  pr["}"];
  pr[];

  (* Jacobian from Cartesian to Cylerical in terms of Cylerical Coordinates *)

  pr["template <typename T>"];
  pr["CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr "
     <> "std::array<std::array<T, 3>, 3>"];
  pr["jac_cart2cyl_cyl(const std::array<T, 3> &xCyl) noexcept {"];
  pr["  const T rh = xCyl[0], ph = xCyl[1];"];
  pr[];
  pr["  const T sp = std::sin(ph);"];
  pr["  const T cp = std::cos(ph);"];
  pr[];
  pr["  const T rhInv = T{1}/rh;"];
  pr[];
  pr["  return {{"];
  Do[
    pr["    {"];
    Do[
      If[ii != 3,
        pr["      "
           <> ToString[CForm[
                (JacCylinC[{kk, cyl}, {ii, -cart}] /. Cart2CylRules // FullSimplify)
                  /. CylToCRules /. PowerToCRules]] <> ","],
        pr["      "
           <> ToString[CForm[
                (JacCylinC[{kk, cyl}, {ii, -cart}] /. Cart2CylRules // FullSimplify)
                  /. CylToCRules /. PowerToCRules]]]
      ],
    {ii, 1, 3}];
    If[kk != 3, pr["    },"], pr["    }"]],
  {kk, 1, 3}];
  pr["  }};"];
  pr["}"];
  pr[];

  (* Jacobian derivatives from Cartesian to Cylerical in terms of Cartesian
   * Coordinates *)

  pr["template <typename T>"];
  pr["CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr "
     <> "std::array<std::array<T, 6>, 3>"];
  pr["djac_cart2cyl_cart(const std::array<T, 3> &xC) {"];
  pr["  const T x = xC[0], y = xC[1], z = xC[2];"];
  pr[];
  pr["  const T x2 = x*x;"];
  pr["  const T y2 = y*y;"];
  pr[];
  pr["  const T rh2 = x2 + y2;"];
  pr["  const T rh = sqrt(rh2);"];
  pr[];
  pr["  const T rhInv = T{1}/rh;"];
  pr["  const T rhInv2 = rhInv*rhInv;"];
  pr["  const T rhInv3 = rhInv2*rhInv;"];
  pr["  const T rhInv4 = rhInv2*rhInv2;"];
  pr[];
  pr["  return {{"];
  Do[
    pr["    {"];
    Do[
      If[ii != 3 || jj != 3,
        pr["      "
           <> ToString[CForm[
                Refine[(PDcart[{jj, -cart}][JacCylinC[{kk, cyl}, {ii, -cart}]]
                  // Simplify) /. CartToCRules,
                {r > 0, rh > 0}] /. CartToCRules /. PowerToCRules]] <> ","],
        pr["      "
           <> ToString[CForm[
                Refine[(PDcart[{jj, -cart}][JacCylinC[{kk, cyl}, {ii, -cart}]]
                  // Simplify) /. CartToCRules,
                {r > 0, rh > 0}] /. CartToCRules /. PowerToCRules]]]
      ],
    {ii, 1, 3}, {jj, ii, 3}];
    If[kk != 3,
      pr["    },"],
      pr["    }"]
    ],
  {kk, 1, 3}];
  pr["  }};"];
  pr["}"];
  pr[];

  (* Jacobian derivatives from Cartesian to Cylerical in terms of Cylerical
   * Coordinates *)

  pr["template <typename T>"];
  pr["CCTK_DEVICE CCTK_HOST CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr "
     <> "std::array<std::array<T, 6>, 3>"];
  pr["djac_cart2cyl_cyl(const std::array<T, 3> &xCyl) {"];
  pr["  const T rh = xCyl[0], ph = xCyl[1];"];
  pr[];
  pr["  const T sp = std::sin(ph);"];
  pr["  const T cp = std::cos(ph);"];
  pr[];
  pr["  const T sp2 = sp*sp;"];
  pr["  const T cp2 = cp*cp;"];
  pr[];
  pr["  const T s2p = T{2}*cp*sp;"];
  pr["  const T c2p = cp2 - sp2;"];
  pr[];
  pr["  const T rhInv = T{1}/rh;"];
  pr["  const T rhInv2 = rhInv*rhInv;"];
  pr[];
  pr["  return {{"];
  Do[
    pr["    {"];
    Do[
      If[ii != 3 || jj != 3,
        pr["      "
           <> ToString[CForm[
                (PDcart[{jj, -cart}][JacCylinC[{kk, cyl}, {ii, -cart}]]
                  /. Cart2CylRules // FullSimplify)
                /. CylToCRules /. PowerToCRules]] <> ","],
        pr["      "
           <> ToString[CForm[
                (PDcart[{jj, -cart}][JacCylinC[{kk, cyl}, {ii, -cart}]]
                  /. Cart2CylRules // FullSimplify)
                /. CylToCRules /. PowerToCRules]]]
      ],
    {ii, 1, 3}, {jj, ii, 3}];
    If[kk != 3,
      pr["    },"],
      pr["    }"]
    ],
  {kk, 1, 3}];
  pr["  }};"];
  pr["}"];
  pr[];

  pr["} // namespace CurvBase"];
];

Import[FileNameJoin[{Environment["GENERATO"], "codes/CarpetXGPU.wl"}]];
