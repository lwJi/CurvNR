#ifndef CURVBASE_PATCH_HXX
#define CURVBASE_PATCH_HXX

/**
 * @file
 * @brief Defines the core Patch struct for the CurvBase multipatch system.
 */

#include <array>
#include <cmath>
#include <variant>

#include "CurvBase_PatchCartesian.hxx"
#include "CurvBase_PatchCubedSphere.hxx"
#include "CurvBase_PatchSpherical.hxx"

namespace CurvBase {

//==============================================================================
// Patch Definition
//==============================================================================

struct L2GVisitor {
  const Coord &l;
  CCTK_HOST CCTK_DEVICE Coord
  operator()(const CartesianMeta &m) const noexcept {
    return cart_l2g(l, &m);
  }
  CCTK_HOST CCTK_DEVICE Coord
  operator()(const SphericalMeta &m) const noexcept {
    return sph_l2g(l, &m);
  }
  CCTK_HOST CCTK_DEVICE Coord
  operator()(const CubedSphereMeta &m) const noexcept {
    return cubedsphere_l2g(l, &m);
  }
};

struct G2LVisitor {
  const Coord &g;
  CCTK_HOST CCTK_DEVICE Coord
  operator()(const CartesianMeta &m) const noexcept {
    return cart_g2l(g, &m);
  }
  CCTK_HOST CCTK_DEVICE Coord
  operator()(const SphericalMeta &m) const noexcept {
    return sph_g2l(g, &m);
  }
  CCTK_HOST CCTK_DEVICE Coord
  operator()(const CubedSphereMeta &m) const noexcept {
    return cubedsphere_g2l(g, &m);
  }
};

struct IsValidVisitor {
  const Coord &l;
  CCTK_HOST CCTK_DEVICE bool operator()(const CartesianMeta &m) const noexcept {
    return cart_valid(l, &m);
  }
  CCTK_HOST CCTK_DEVICE bool operator()(const SphericalMeta &m) const noexcept {
    return sph_valid(l, &m);
  }
  CCTK_HOST CCTK_DEVICE bool
  operator()(const CubedSphereMeta &m) const noexcept {
    return cubedsphere_valid(l, &m);
  }
};

struct FaceInfo {
  bool is_outer_boundary{true};
};

/**
 * @brief Represents a single computational grid in a larger multipatch system.
 *
 * A Patch defines a logical region of space with a specific coordinate
 * mapping (e.g., Cartesian, Spherical). It holds the grid structure
 * (cell counts, bounds) and provides functions to transform coordinates
 * between its local frame and a global Cartesian frame.
 *
 * The struct is designed to be a Plain Old Data (POD-like) type to allow
 * it to be easily copied to and used in GPU global memory.
 */
struct Patch {

  std::variant<CartesianMeta, SphericalMeta, CubedSphereMeta> meta{};
  Index ncells{};
  Coord xmin{}, xmax{}, dx{};
  std::array<std::array<FaceInfo, dim>, 2> faces{};

  CCTK_HOST Patch() = default;

  template <class MetaT>
  CCTK_HOST constexpr Patch(MetaT meta_in, Index nc, Coord lo,
                            Coord hi) noexcept
      : meta(std::move(meta_in)), ncells{nc}, xmin{lo}, xmax{hi} {
    for (std::size_t d = 0; d < dim; ++d) {
      assert(ncells[d] > 0 && "ncells must be positive");
      assert(xmax[d] > xmin[d] && "xmax must exceed xmin");
      dx[d] = (xmax[d] - xmin[d]) / static_cast<CCTK_REAL>(ncells[d]);
    }
  }

  [[nodiscard]] CCTK_HOST CCTK_DEVICE Coord l2g(const Coord &l) const noexcept {
    return std::visit(L2GVisitor{l}, meta);
  }

  [[nodiscard]] CCTK_HOST CCTK_DEVICE Coord g2l(const Coord &g) const noexcept {
    return std::visit(G2LVisitor{g}, meta);
  }

  [[nodiscard]] CCTK_HOST CCTK_DEVICE bool
  is_valid(const Coord &l) const noexcept {
    return std::visit(IsValidVisitor{l}, meta);
  }
};

//==============================================================================
// Factory Function
//==============================================================================

/**
 * @brief A factory function to simplify the creation of Patch objects.
 *
 * This helper function forwards arguments to the Patch constructor, providing
 * a cleaner syntax for instantiation.
 *
 * @tparam MetaT The coordinate metadata type.
 * @tparam MetaArgs Argument types for the metadata constructor.
 * @return A fully constructed Patch object.
 */
template <class MetaT, class... MetaArgs>
[[nodiscard]] CCTK_HOST constexpr auto
make_patch(Index ncells, Coord xmin, Coord xmax,
           MetaArgs &&...meta_args) noexcept {
  static_assert(std::is_constructible_v<MetaT, MetaArgs...>,
                "MetaT cannot be constructed from supplied args");
  return Patch(MetaT(std::forward<MetaArgs>(meta_args)...), ncells, xmin, xmax);
}

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCH_HXX
