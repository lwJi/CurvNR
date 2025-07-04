#ifndef CURVBASE_PATCH_HXX
#define CURVBASE_PATCH_HXX

#include <array>
#include <cmath>

#include "CurvBase_PatchCartesian.hxx"
#include "CurvBase_PatchCubedSphere.hxx"
#include "CurvBase_PatchSpherical.hxx"

namespace CurvBase {

using Coord = std::array<CCTK_REAL, 3>;

struct PatchMap {
  CCTK_HOST CCTK_DEVICE Coord (*l2g)(const Coord &, const void *) = nullptr;
  CCTK_HOST CCTK_DEVICE Coord (*g2l)(const Coord &, const void *) = nullptr;
  CCTK_HOST CCTK_DEVICE bool (*is_valid)(const Coord &) = nullptr;
};

struct FaceInfo {
  bool is_outer_boundary{true};
};

struct Patch {
  // metadata container
  std::variant<CartesianMeta, SphericalMeta, CubedSphereMeta> meta{};

  // helper: return pointer to the **concrete** meta inside the variant
  const void *meta_ptr() const noexcept {
    return std::visit([](auto const &m) -> const void * { return &m; }, meta);
  }

  PatchMap map{};

  std::array<CCTK_INT, dim> ncells{}; // cells per dimension
  std::array<CCTK_REAL, dim> xmin{};  // lower physical bounds
  std::array<CCTK_REAL, dim> xmax{};  // upper physical bounds

  std::array<std::array<FaceInfo, dim>, 2> faces{};

  bool is_cartesian{false};
};

//------------------------------------------------------------------------------
// Helpers that create individual Patch objects
//------------------------------------------------------------------------------
inline Patch make_cart_patch() {
  Patch p;
  p.map = {cart_l2g, cart_g2l, cart_valid};
  p.meta = CartesianMeta{}; // active alt set
  p.is_cartesian = true;
  return p;
}

inline Patch make_sph_patch(CCTK_REAL r0, CCTK_REAL r1) {
  Patch p;
  p.map = {sph_l2g, sph_g2l, sph_valid};
  p.meta = SphericalMeta{r0, r1};
  p.is_cartesian = false;
  return p;
}

inline Patch make_wedge(Face f, CCTK_REAL r0, CCTK_REAL r1) {
  Patch p;
  p.map = {cubedsphere_l2g, cubedsphere_g2l, cubedsphere_valid};
  p.meta = CubedSphereMeta{f, r0, r1};
  p.is_cartesian = false;
  return p;
}

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCH_HXX
