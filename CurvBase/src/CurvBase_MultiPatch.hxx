#ifndef CURVBASE_MULTIPATCH_HXX
#define CURVBASE_MULTIPATCH_HXX

/**
 * @file
 * @brief Defines the MultiPatch container and the global instance for the
 * system.
 */

#include <AMReX_Gpu.H>

#include <variant>

#include "CurvBase_Patch.hxx"

namespace CurvBase {

//==============================================================================
// MultiPatch Definition
//==============================================================================

/**
 * @brief Manages a collection of Patches for a composite grid geometry.
 *
 * This class serves two primary roles:
 * 1. A container for a fixed-size array of `Patch` objects, providing
 * device-callable coordinate transformations between local and global frames.
 * 2. A host-side factory for building specific, pre-defined grid
 * configurations (e.g., Cartesian, Cubed-Sphere).
 *
 * @tparam MaxP The maximum number of patches this container can hold.
 */
template <std::size_t MaxP> class MultiPatch {
  Patch patches_[MaxP]{};
  std::size_t count_{0};

  static constexpr Coord invalid_coord = {0.0, 0.0, 0.0};
  static constexpr std::size_t invalid_patch_id = static_cast<std::size_t>(-1);

public:
  CCTK_HOST MultiPatch() = default;

  CCTK_HOST bool add_patch(const Patch &p) {
    if (count_ >= MaxP)
      return false;
    patches_[count_++] = p;
    return true;
  }

  [[nodiscard]] CCTK_HOST CCTK_DEVICE const Patch *
  get_patch(std::size_t id) const noexcept {
    return (id < count_) ? &patches_[id] : nullptr;
  }

  [[nodiscard]] CCTK_HOST CCTK_DEVICE std::size_t size() const noexcept {
    return count_;
  }

  [[nodiscard]] CCTK_HOST CCTK_DEVICE Coord l2g(std::size_t id,
                                                const Coord &l) const noexcept {
    return (id < count_) ? patches_[id].l2g(l) : invalid_coord;
  }

  [[nodiscard]] CCTK_HOST CCTK_DEVICE std::pair<Coord, std::size_t>
  g2l(const Coord &g) const noexcept {
    for (std::size_t i = 0; i < count_; ++i) {
      const Coord loc = patches_[i].g2l(g);
      if (patches_[i].is_valid(loc)) {
        return {loc, i};
      }
    }
    return {invalid_coord, invalid_patch_id};
  }

  // --- Host-Side Factory Methods ---

  CCTK_HOST void select_cartesian(Index ncells, Coord xmin, Coord xmax) {
    count_ = 0;
    add_patch(make_patch<CartesianMeta>(ncells, xmin, xmax));
  }

  CCTK_HOST void select_spherical(Index ncells, Coord xmin, Coord xmax) {
    count_ = 0;
    add_patch(make_patch<SphericalMeta>(ncells, xmin, xmax));
  }

  CCTK_HOST void select_cubedsphere(Index ncells, Coord xmin, Coord xmax,
                                    CCTK_REAL r0, CCTK_REAL r1) {
    count_ = 0;
    add_patch(make_patch<CartesianMeta>(ncells, xmin, xmax));
    for (auto w :
         {Wedge::PX, Wedge::NX, Wedge::PY, Wedge::NY, Wedge::PZ, Wedge::NZ}) {
      add_patch(make_patch<CubedSphereMeta>(ncells, xmin, xmax, w, r0, r1));
    }
  }
};

//==============================================================================
// Global Multipatch Instance
//==============================================================================

using AMP = MultiPatch<10>;

AMREX_GPU_MANAGED AMP g_active_mp;

inline AMP &active_mp() { return g_active_mp; }

} // namespace CurvBase

#endif // #ifndef CURVBASE_MULTIPATCH_HXX
