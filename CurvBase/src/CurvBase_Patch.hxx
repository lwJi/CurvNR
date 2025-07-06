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

/**
 * @brief Holds information about a single face of a patch.
 */
struct FaceInfo {
  /// @brief True if this face is an outer boundary of the entire domain.
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

  // --- Member Data -----------------------------------------------------------
  // This data is structured to be GPU-friendly.

  /// @brief The coordinate system metadata, determining the mapping for this
  /// patch.
  std::variant<CartesianMeta, SphericalMeta, CubedSphereMeta> meta{};

  /// @brief The number of grid cells in each logical dimension (i, j, k).
  Index ncells{};
  /// @brief The lower bounds of the patch in its own local coordinates.
  Coord xmin{};
  /// @brief The upper bounds of the patch in its own local coordinates.
  Coord xmax{};
  /// @brief The pre-calculated cell spacing in each local dimension.
  Coord dx{};

  /// @brief Information for each of the 6 faces (2 per dimension: low, high).
  std::array<std::array<FaceInfo, dim>, 2> faces{};

  // --- Construction ----------------------------------------------------------

  /// @brief Default constructor for array initialization.
  CCTK_HOST Patch() = default;

  /**
   * @brief Constructs a fully initialized Patch on the host.
   *
   * @tparam MetaT The type of the coordinate metadata (e.g., CartesianMeta).
   * @tparam MetaArgs Argument types for the metadata constructor.
   * @param meta_in An instance of the metadata.
   * @param nc The number of cells per dimension.
   * @param lo The lower bounds in local coordinates.
   * @param hi The upper bounds in local coordinates.
   * @param rest Additional arguments for the metadata constructor.
   */
  template <class MetaT, class... MetaArgs>
  CCTK_HOST constexpr Patch(MetaT meta_in, Index nc, Coord lo, Coord hi,
                            MetaArgs &&...rest) noexcept
      : meta(std::in_place_type<MetaT>, std::move(meta_in),
             std::forward<MetaArgs>(rest)...),
        ncells{nc}, xmin{lo}, xmax{hi} {
    for (std::size_t d = 0; d < dim; ++d) {
      assert(ncells[d] > 0 && "ncells must be positive");
      assert(xmax[d] > xmin[d] && "xmax must exceed xmin");
      dx[d] = (xmax[d] - xmin[d]) / static_cast<CCTK_REAL>(ncells[d]);
    }
  }

  //--- Coordinate Transformations ---------------------------------------------
  // The following methods use std::visit on the `meta` variant to dispatch
  // to the correct, statically-typed transformation function.

  /**
   * @brief Transforms local coordinates within this patch to the global frame.
   * @param l A point in the local coordinate system of this patch.
   * @return The corresponding point in the global Cartesian frame.
   */
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

  /**
   * @brief Transforms global coordinates into the local frame of this patch.
   * @param g A point in the global Cartesian frame.
   * @return The corresponding point in this patch's local coordinate system.
   */
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

  /**
   * @brief Checks if a local coordinate point is within the valid domain.
   *
   * For example, a cubed-sphere patch is only valid for local coordinates
   * within [-1, 1] for the tangential dimensions.
   *
   * @param l A point in the local coordinate system of this patch.
   * @return True if the point is within the patch's valid logical domain.
   */
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
[[nodiscard]] constexpr Patch make_patch(Index ncells, Coord xmin, Coord xmax,
                                         MetaArgs &&...meta_args) {
  static_assert(std::is_constructible_v<MetaT, MetaArgs...>,
                "Meta cannot be constructed from supplied args");
  return Patch{MetaT(std::forward<MetaArgs>(meta_args)...), ncells, xmin, xmax};
}

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCH_HXX
