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

  // Geometry (POD so it can live in GPU global memory)

  std::variant<CartesianMeta, SphericalMeta, CubedSphereMeta> meta{};

  Index ncells{}; // cells per dimension
  Coord xmin{};   // lower physical bounds
  Coord xmax{};   // upper physical bounds
  Coord dx{};

  std::array<std::array<FaceInfo, dim>, 2> faces{};

  // Single constructor – guarantees full initialisation

  CCTK_HOST Patch() = default;

  template <class MetaT, class... MetaArgs>
  CCTK_HOST constexpr Patch(MetaT meta_in, Index nc, Coord lo, Coord hi,
                            MetaArgs &&...rest) noexcept
      : meta(std::in_place_type<MetaT>, std::forward<MetaT>(meta_in),
             std::forward<MetaArgs>(rest)...),
        ncells{nc}, xmin{lo}, xmax{hi} {
    for (std::size_t d = 0; d < dim; ++d) {
      assert(ncells[d] > 0 && "ncells must be positive");
      assert(xmax[d] > xmin[d] && "xmax must exceed xmin");
      dx[d] = (xmax[d] - xmin[d]) / static_cast<CCTK_REAL>(ncells[d]);
    }
  }

  // Coordinate transfromation

  // local -> global
  [[nodiscard]] CCTK_HOST CCTK_DEVICE Coord l2g(Coord const &l) const noexcept {
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
  [[nodiscard]] CCTK_HOST CCTK_DEVICE Coord g2l(Coord const &g) const noexcept {
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
  [[nodiscard]] CCTK_HOST CCTK_DEVICE bool
  is_valid(Coord const &l) const noexcept {
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
};

//------------------------------------------------------------------------------
// Helpers that create individual Patch objects
//------------------------------------------------------------------------------

template <class MetaT, class... MetaArgs>
[[nodiscard]] constexpr Patch make_patch(Index ncells, Coord xmin, Coord xmax,
                                         MetaArgs &&...meta_args) {
  static_assert(std::is_constructible_v<MetaT, MetaArgs...>,
                "Meta cannot be constructed from supplied args");
  return Patch{MetaT(std::forward<MetaArgs>(meta_args)...), ncells, xmin, xmax};
}

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCH_HXX
