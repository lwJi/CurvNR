#ifndef CURVBASE_PATCH_HXX
#define CURVBASE_PATCH_HXX

/**
 * @file
 * @brief Defines the core Patch struct for the CurvBase multipatch system.
 */

#include <array>
#include <cmath>
#include <type_traits>
#include <utility>

#include "CurvBase_PatchCartesian.hxx"
#include "CurvBase_PatchCubedSphereWedge.hxx"
#include "CurvBase_PatchSpherical.hxx"

namespace CurvBase {

template <class T, class... Ts>
inline constexpr bool is_any_v = (std::is_same_v<T, Ts> || ...);

[[noreturn]] CCTK_HOST CCTK_DEVICE inline void unreachable() {
#if defined(__GNUC__) || defined(__clang__)
  __builtin_unreachable();
#elif defined(_MSC_VER)
  __assume(0);
#endif
}

//==============================================================================
// Patch Definition
//==============================================================================

enum class PatchType { Cartesian, Spherical, CubedSphereWedge };

union MetaUnion {
  CartesianMeta cart;
  SphericalMeta sph;
  CubedSphereWedgeMeta cs_wedge;

  // Constructors for host-side convenience
  CCTK_HOST constexpr MetaUnion() : cart{} {} // Default constructor
  CCTK_HOST constexpr MetaUnion(const CartesianMeta &m) : cart(m) {}
  CCTK_HOST constexpr MetaUnion(const SphericalMeta &m) : sph(m) {}
  CCTK_HOST constexpr MetaUnion(const CubedSphereWedgeMeta &m) : cs_wedge(m) {}
};

static_assert(
    std::is_trivially_copyable_v<MetaUnion>,
    "MetaUnion must stay POD so the whole Patch is trivially copyable");

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

  PatchType type;
  MetaUnion meta;

  Index ncells{};
  Coord xmin{}, xmax{}, dx{};
  PatchFaces faces{};

  CCTK_HOST Patch() = default;

  template <class MetaT>
  CCTK_HOST constexpr Patch(MetaT meta_in, Index nc, Coord lo, Coord hi,
                            const PatchFaces &faces_in) noexcept
      : meta(std::move(meta_in)), ncells{nc}, xmin{lo}, xmax{hi},
        faces{faces_in} {
    // Set the patch type based on the metadata type provided
    if constexpr (std::is_same_v<MetaT, CartesianMeta>) {
      type = PatchType::Cartesian;
    } else if constexpr (std::is_same_v<MetaT, SphericalMeta>) {
      type = PatchType::Spherical;
    } else if constexpr (std::is_same_v<MetaT, CubedSphereWedgeMeta>) {
      type = PatchType::CubedSphereWedge;
    }

    static_assert(
        is_any_v<MetaT, CartesianMeta, SphericalMeta, CubedSphereWedgeMeta>,
        "Unsupported MetaT");

    for (std::size_t d = 0; d < dim; ++d) {
      assert(ncells[d] > 0 && "ncells must be positive");
      assert(xmax[d] > xmin[d] && "xmax must exceed xmin");
      dx[d] = (xmax[d] - xmin[d]) / static_cast<CCTK_REAL>(ncells[d]);
    }
  }

private:
  template <typename Visitor>
  [[nodiscard]]
  CCTK_HOST CCTK_DEVICE CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr decltype(auto)
  visit_meta(Visitor &&visitor) const {
    switch (type) {
    case PatchType::Cartesian:
      return std::forward<Visitor>(visitor)(meta.cart);
    case PatchType::Spherical:
      return std::forward<Visitor>(visitor)(meta.sph);
    case PatchType::CubedSphereWedge:
      return std::forward<Visitor>(visitor)(meta.cs_wedge);
    default:
      unreachable(); // never returns
    }
  }

  // l2g
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE auto
  l2g_impl(const Coord &l, const CartesianMeta &m) {
    return cart_l2g(l, &m);
  }
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE auto
  l2g_impl(const Coord &l, const SphericalMeta &m) {
    return sph_l2g(l, &m);
  }
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE auto
  l2g_impl(const Coord &l, const CubedSphereWedgeMeta &m) {
    return cubedspherewedge_l2g(l, &m);
  }

  // g2l
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE auto
  g2l_impl(const Coord &g, const CartesianMeta &m) {
    return cart_g2l(g, &m);
  }
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE auto
  g2l_impl(const Coord &g, const SphericalMeta &m) {
    return sph_g2l(g, &m);
  }
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE auto
  g2l_impl(const Coord &g, const CubedSphereWedgeMeta &m) {
    return cubedspherewedge_g2l(g, &m);
  }

  // is_valid
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE bool
  is_valid_impl(const Coord &l, const CartesianMeta &m) {
    return cart_valid(l, &m);
  }
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE bool
  is_valid_impl(const Coord &l, const SphericalMeta &m) {
    return sph_valid(l, &m);
  }
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE bool
  is_valid_impl(const Coord &l, const CubedSphereWedgeMeta &m) {
    return cubedspherewedge_valid(l, &m);
  }

  // jac_g2l_g
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE constexpr Jac_t
  jac_g2l_g_impl(const Coord &g, const CartesianMeta &) {
    return jac_cart2cart_cart(g);
  }
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE constexpr Jac_t
  jac_g2l_g_impl(const Coord &g, const SphericalMeta &) {
    return jac_cart2sph_cart(g);
  }
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE constexpr Jac_t
  jac_g2l_g_impl(const Coord &g, const CubedSphereWedgeMeta &) {
    return jac_cart2wedge_cart(g);
  }

  // jac_g2l_l
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE constexpr Jac_t
  jac_g2l_l_impl(const Coord &l, const CartesianMeta &) {
    return jac_cart2cart_cart(l);
  }
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE constexpr Jac_t
  jac_g2l_l_impl(const Coord &l, const SphericalMeta &) {
    return jac_cart2sph_sph(l);
  }
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE constexpr Jac_t
  jac_g2l_l_impl(const Coord &l, const CubedSphereWedgeMeta &) {
    return jac_cart2wedge_wedge(l);
  }

  // djac_g2l_g
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE constexpr dJac_t
  djac_g2l_g_impl(const Coord &g, const CartesianMeta &) {
    return djac_cart2cart_cart(g);
  }
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE constexpr dJac_t
  djac_g2l_g_impl(const Coord &g, const SphericalMeta &) {
    return djac_cart2sph_cart(g);
  }
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE constexpr dJac_t
  djac_g2l_g_impl(const Coord &g, const CubedSphereWedgeMeta &) {
    return djac_cart2wedge_cart(g);
  }

  // djac_g2l_l
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE constexpr dJac_t
  djac_g2l_l_impl(const Coord &l, const CartesianMeta &) {
    return djac_cart2cart_cart(l);
  }
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE constexpr dJac_t
  djac_g2l_l_impl(const Coord &l, const SphericalMeta &) {
    return djac_cart2sph_sph(l);
  }
  [[nodiscard]] static CCTK_HOST CCTK_DEVICE constexpr dJac_t
  djac_g2l_l_impl(const Coord &l, const CubedSphereWedgeMeta &) {
    return djac_cart2wedge_wedge(l);
  }

public:
  [[nodiscard]] CCTK_HOST CCTK_DEVICE Coord l2g(const Coord &l) const noexcept {
    return visit_meta([&](const auto &m) { return l2g_impl(l, m); });
  }

  [[nodiscard]] CCTK_HOST CCTK_DEVICE Coord g2l(const Coord &g) const noexcept {
    return visit_meta([&](const auto &m) { return g2l_impl(g, m); });
  }

  [[nodiscard]] CCTK_HOST CCTK_DEVICE bool
  is_valid(const Coord &l) const noexcept {
    return visit_meta([&](const auto &m) { return is_valid_impl(l, m); });
  }

  [[nodiscard]] CCTK_HOST
      CCTK_DEVICE CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr Jac_t
      jac_g2l_g(const Coord &g) const noexcept {
    return visit_meta([&](const auto &m) { return jac_g2l_g_impl(g, m); });
  }

  [[nodiscard]] CCTK_HOST
      CCTK_DEVICE CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr Jac_t
      jac_g2l_l(const Coord &l) const noexcept {
    return visit_meta([&](const auto &m) { return jac_g2l_l_impl(l, m); });
  }

  [[nodiscard]] CCTK_HOST
      CCTK_DEVICE CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr dJac_t
      djac_g2l_g(const Coord &g) const noexcept {
    return visit_meta([&](const auto &m) { return djac_g2l_g_impl(g, m); });
  }

  [[nodiscard]] CCTK_HOST
      CCTK_DEVICE CCTK_ATTRIBUTE_ALWAYS_INLINE constexpr dJac_t
      djac_g2l_l(const Coord &l) const noexcept {
    return visit_meta([&](const auto &m) { return djac_g2l_l_impl(l, m); });
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
