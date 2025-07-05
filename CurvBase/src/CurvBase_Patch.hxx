#ifndef CURVBASE_PATCH_HXX
#define CURVBASE_PATCH_HXX

#include <array>
#include <cmath>

#include "CurvBase_PatchCartesian.hxx"
#include "CurvBase_PatchCubedSphere.hxx"
#include "CurvBase_PatchSpherical.hxx"

namespace CurvBase {

//------------------------------------------------------------------------------
// Patch
//------------------------------------------------------------------------------

struct FaceInfo {
  bool is_outer_boundary{true};
};

struct Patch {
  // metadata container
  std::variant<CartesianMeta, SphericalMeta, CubedSphereMeta> meta{};

  // local -> global
  CCTK_HOST CCTK_DEVICE Coord l2g(Coord const &l) const noexcept {
    return std::visit(
        [&](auto const &m) noexcept {
          if constexpr (std::is_same_v<decltype(m), CartesianMeta const &>)
            return cart_l2g(l, &m);
          else if constexpr (std::is_same_v<decltype(m), SphericalMeta const &>)
            return sph_l2g(l, &m);
          else
            return cubedsphere_l2g(l, &m);
        },
        meta);
  }

  // global -> local
  CCTK_HOST CCTK_DEVICE Coord g2l(Coord const &g) const noexcept {
    return std::visit(
        [&](auto const &m) noexcept {
          if constexpr (std::is_same_v<decltype(m), CartesianMeta const &>)
            return cart_g2l(g, &m);
          else if constexpr (std::is_same_v<decltype(m), SphericalMeta const &>)
            return sph_g2l(g, &m);
          else
            return cubedsphere_g2l(g, &m);
        },
        meta);
  }

  // validity
  CCTK_HOST CCTK_DEVICE bool is_valid(Coord const &l) const noexcept {
    return std::visit(
        [&](auto const &m) noexcept {
          if constexpr (std::is_same_v<decltype(m), CartesianMeta const &>)
            return cart_valid(l, &m);
          else if constexpr (std::is_same_v<decltype(m), SphericalMeta const &>)
            return sph_valid(l, &m);
          else
            return cubedsphere_valid(l, &m);
        },
        meta);
  }

  std::array<CCTK_INT, dim> ncells{}; // cells per dimension
  std::array<CCTK_REAL, dim> xmin{};  // lower physical bounds
  std::array<CCTK_REAL, dim> xmax{};  // upper physical bounds

  std::array<std::array<FaceInfo, dim>, 2> faces{};

  bool is_cartesian{false};
};

//------------------------------------------------------------------------------
// Helpers that create individual Patch objects
//------------------------------------------------------------------------------

template <typename Meta, typename... Args> Patch make_patch(Args &&...args) {
  Patch p;
  p.meta.emplace<Meta>(std::forward<Args>(args)...);
  return p;
}

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCH_HXX
