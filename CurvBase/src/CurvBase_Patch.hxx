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
#include "CurvBase_PatchCubedSphereWedge.hxx"
#include "CurvBase_PatchSpherical.hxx"

namespace CurvBase {

//==============================================================================
// Patch Definition
//==============================================================================

namespace detail {
template <class T> struct always_false : std::false_type {};
} // namespace detail

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

  std::variant<CartesianMeta, SphericalMeta, CubedSphereWedgeMeta> meta{};
  Index ncells{};
  Coord xmin{}, xmax{}, dx{};
  PatchFaces faces{};

  CCTK_HOST Patch() = default;

  template <class MetaT>
  CCTK_HOST constexpr Patch(MetaT meta_in, Index nc, Coord lo, Coord hi,
                            const PatchFaces &faces_in) noexcept
      : meta(std::move(meta_in)), ncells{nc}, xmin{lo}, xmax{hi},
        faces{faces_in} {
    for (std::size_t d = 0; d < dim; ++d) {
      assert(ncells[d] > 0 && "ncells must be positive");
      assert(xmax[d] > xmin[d] && "xmax must exceed xmin");
      dx[d] = (xmax[d] - xmin[d]) / static_cast<CCTK_REAL>(ncells[d]);
    }
  }

  [[nodiscard]] CCTK_HOST CCTK_DEVICE Coord l2g(const Coord &l) const noexcept {
    return std::visit(
        [&l](const auto &m) constexpr noexcept -> Coord {
          using MetaT = std::decay_t<decltype(m)>;
          if constexpr (std::is_same_v<MetaT, CartesianMeta>) {
            return cart_l2g(l, &m);
          } else if constexpr (std::is_same_v<MetaT, SphericalMeta>) {
            return sph_l2g(l, &m);
          } else if constexpr (std::is_same_v<MetaT, CubedSphereWedgeMeta>) {
            return cubedspherewedge_l2g(l, &m);
          } else {
            static_assert(detail::always_false<MetaT>::value,
                          "Unhandled meta type");
            return {};
          }
        },
        meta);
  }

  [[nodiscard]] CCTK_HOST CCTK_DEVICE Coord g2l(const Coord &g) const noexcept {
    return std::visit(
        [&g](const auto &m) constexpr noexcept -> Coord {
          using MetaT = std::decay_t<decltype(m)>;
          if constexpr (std::is_same_v<MetaT, CartesianMeta>) {
            return cart_g2l(g, &m);
          } else if constexpr (std::is_same_v<MetaT, SphericalMeta>) {
            return sph_g2l(g, &m);
          } else if constexpr (std::is_same_v<MetaT, CubedSphereWedgeMeta>) {
            return cubedspherewedge_g2l(g, &m);
          } else {
            static_assert(detail::always_false<MetaT>::value,
                          "Unhandled meta type");
            return {};
          }
        },
        meta);
  }

  [[nodiscard]] CCTK_HOST CCTK_DEVICE bool
  is_valid(const Coord &l) const noexcept {
    return std::visit(
        [&l](const auto &m) constexpr noexcept -> bool {
          using MetaT = std::decay_t<decltype(m)>;
          if constexpr (std::is_same_v<MetaT, CartesianMeta>) {
            return cart_valid(l, &m);
          } else if constexpr (std::is_same_v<MetaT, SphericalMeta>) {
            return sph_valid(l, &m);
          } else if constexpr (std::is_same_v<MetaT, CubedSphereWedgeMeta>) {
            return cubedspherewedge_valid(l, &m);
          } else {
            static_assert(detail::always_false<MetaT>::value,
                          "Unhandled meta type");
            return {};
          }
        },
        meta);
  }

  [[nodiscard]] CCTK_HOST
      CCTK_DEVICE CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr Jac_t
      jac_g2l_g(const Coord &g) const noexcept {
    return std::visit(
        [&g](const auto &m) constexpr noexcept -> Jac_t {
          using MetaT = std::decay_t<decltype(m)>;
          if constexpr (std::is_same_v<MetaT, CartesianMeta>) {
            return jac_cart2cart_cart(g);
          } else if constexpr (std::is_same_v<MetaT, SphericalMeta>) {
            return jac_cart2sph_cart(g);
          } else if constexpr (std::is_same_v<MetaT, CubedSphereWedgeMeta>) {
            return jac_cart2wedge_cart(g);
          } else {
            static_assert(detail::always_false<MetaT>::value,
                          "Unhandled meta type");
            return {};
          }
        },
        meta);
  }

  [[nodiscard]] CCTK_HOST
      CCTK_DEVICE CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr Jac_t
      jac_g2l_l(const Coord &g) const noexcept {
    return std::visit(
        [&g](const auto &m) constexpr noexcept -> Jac_t {
          using MetaT = std::decay_t<decltype(m)>;
          if constexpr (std::is_same_v<MetaT, CartesianMeta>) {
            return jac_cart2cart_cart(g);
          } else if constexpr (std::is_same_v<MetaT, SphericalMeta>) {
            return jac_cart2sph_sph(g);
          } else if constexpr (std::is_same_v<MetaT, CubedSphereWedgeMeta>) {
            return jac_cart2wedge_wedge(g);
          } else {
            static_assert(detail::always_false<MetaT>::value,
                          "Unhandled meta type");
            return {};
          }
        },
        meta);
  }

  [[nodiscard]] CCTK_HOST
      CCTK_DEVICE CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr dJac_t
      djac_g2l_g(const Coord &g) const noexcept {
    return std::visit(
        [&g](const auto &m) constexpr noexcept -> dJac_t {
          using MetaT = std::decay_t<decltype(m)>;
          if constexpr (std::is_same_v<MetaT, CartesianMeta>) {
            return djac_cart2cart_cart(g);
          } else if constexpr (std::is_same_v<MetaT, SphericalMeta>) {
            return djac_cart2sph_cart(g);
          } else if constexpr (std::is_same_v<MetaT, CubedSphereWedgeMeta>) {
            return djac_cart2wedge_cart(g);
          } else {
            static_assert(detail::always_false<MetaT>::value,
                          "Unhandled meta type");
            return {};
          }
        },
        meta);
  }

  [[nodiscard]] CCTK_HOST
      CCTK_DEVICE CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr dJac_t
      djac_g2l_l(const Coord &g) const noexcept {
    return std::visit(
        [&g](const auto &m) constexpr noexcept -> dJac_t {
          using MetaT = std::decay_t<decltype(m)>;
          if constexpr (std::is_same_v<MetaT, CartesianMeta>) {
            return djac_cart2cart_cart(g);
          } else if constexpr (std::is_same_v<MetaT, SphericalMeta>) {
            return djac_cart2sph_sph(g);
          } else if constexpr (std::is_same_v<MetaT, CubedSphereWedgeMeta>) {
            return djac_cart2wedge_wedge(g);
          } else {
            static_assert(detail::always_false<MetaT>::value,
                          "Unhandled meta type");
            return {};
          }
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
[[nodiscard]] CCTK_HOST constexpr auto
make_patch(Index ncells, Coord xmin, Coord xmax, const PatchFaces &faces,
           MetaArgs &&...meta_args) noexcept {
  static_assert(std::is_constructible_v<MetaT, MetaArgs...>,
                "MetaT cannot be constructed from supplied args");
  return Patch(MetaT(std::forward<MetaArgs>(meta_args)...), ncells, xmin, xmax,
               faces);
}

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCH_HXX
