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
  CCTK_HOST CCTK_DEVICE Coord (*local_to_global)(const Coord &,
                                                 const void *) = nullptr;
  CCTK_HOST CCTK_DEVICE Coord (*global_to_local)(const Coord &,
                                                 const void *) = nullptr;
  CCTK_HOST CCTK_DEVICE bool (*is_valid_local)(const Coord &) = nullptr;
};

struct FaceInfo {
  bool is_outer_boundary{true};
};

struct Patch {
  // metadata container
  std::variant<CartesianMeta, SphericalMeta, CubedSphereMeta> meta{};

  // helper: return pointer to the **concrete** meta inside the variant
  CCTK_HOST const void *meta_ptr() const noexcept {
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
  p.map = {cart_local_to_global, cart_global_to_local, cart_valid};
  p.meta = CartesianMeta{}; // active alt set
  p.is_cartesian = true;
  return p;
}

inline Patch make_sph_patch(CCTK_REAL r0, CCTK_REAL r1) {
  Patch p;
  p.map = {sph_local_to_global, sph_global_to_local, sph_valid};
  p.meta = SphericalMeta{r0, r1};
  p.is_cartesian = false;
  return p;
}

inline Patch make_wedge(Face f, CCTK_REAL r0, CCTK_REAL r1) {
  Patch p;
  p.map = {cubedsphere_local_to_global, cubedsphere_global_to_local,
           cubedsphere_valid};
  p.meta = CubedSphereMeta{f, r0, r1};
  p.is_cartesian = false;
  return p;
}

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCH_HXX
