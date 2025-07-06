#ifndef CURVBASE_MULTIPATCH_HXX
#define CURVBASE_MULTIPATCH_HXX

#include <AMReX_Gpu.H>

#include <variant>

#include "CurvBase_Patch.hxx"

namespace CurvBase {

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
  /// @brief Fixed-size storage for all patch data.
  Patch patches_[MaxP]{};
  /// @brief The current number of active patches in the system.
  std::size_t count_{0};

public:
  CCTK_HOST MultiPatch() = default;

  /**
   * @brief Adds a new patch to the system.
   * @param p The Patch to add.
   * @return The ID of the newly added patch, or `MaxP` if the container is
   * full.
   */
  CCTK_HOST std::size_t add_patch(const Patch &p) {
    return (count_ < MaxP) ? (patches_[count_] = p, count_++) : MaxP;
  }

  /**
   * @brief Retrieves a read-only pointer to a patch by its ID.
   * @param id The ID of the patch to retrieve.
   * @return A const pointer to the `Patch`, or `nullptr` if the ID is invalid.
   */
  CCTK_HOST CCTK_DEVICE const Patch *get_patch(std::size_t id) const noexcept {
    return (id < count_) ? &patches_[id] : nullptr;
  }

  /**
   * @brief Transforms local coordinates for a specific patch ID to global
   * coordinates.
   * @param id The ID of the patch whose local frame is being used.
   * @param l The local coordinates within the specified patch.
   * @return The corresponding global coordinates. Returns {0,0,0} if ID is
   * invalid.
   */
  CCTK_HOST CCTK_DEVICE Coord l2g(std::size_t id,
                                  const Coord &l) const noexcept {
    return (id < count_) ? patches_[id].l2g(l) : Coord{0, 0, 0};
  }

  /**
   * @brief Finds the owning patch and local coordinates for a global point.
   *
   * This function iterates through all active patches to find the first one
   * that contains the given global point `g`.
   *
   * @param g The global coordinates to transform.
   * @return A pair containing:
   * - The corresponding local coordinates within the found patch.
   * - The `std::size_t` ID of the found patch.
   * If no patch is found, the ID is set to a sentinel value of -1.
   */
  CCTK_HOST CCTK_DEVICE std::pair<Coord, std::size_t>
  g2l(const Coord &g) const noexcept {
    for (std::size_t i = 0; i < count_; ++i) {
      const Coord loc = patches_[i].g2l(g);
      if (patches_[i].is_valid(loc)) {
        return {loc, i};
      }
    }
    return {Coord{}, static_cast<std::size_t>(-1)};
  }

  /// @brief Returns the number of active patches in the system.
  CCTK_HOST CCTK_DEVICE std::size_t size() const noexcept { return count_; }

  //--- Host-Side Factory Methods ----------------------------------------------
  // These methods configure the MultiPatch object into specific, predefined
  // grid layouts. They are intended to be called only from host code during
  // the simulation setup phase.

  /**
   * @brief Configures the system as a single Cartesian patch.
   * @note This is a host-only function that resets the object.
   */
  CCTK_HOST void select_cartesian(Index ncells, Coord xmin, Coord xmax) {
    *this = {};
    add_patch(make_patch<CartesianMeta>(ncells, xmin, xmax));
  }

  /**
   * @brief Configures the system as a single Spherical patch.
   * @note This is a host-only function that resets the object.
   */
  CCTK_HOST void select_spherical(Index ncells, Coord xmin, Coord xmax) {
    *this = {};
    add_patch(make_patch<SphericalMeta>(ncells, xmin, xmax));
  }

  /**
   * @brief Configures the system as a Cubed-Sphere grid.
   *
   * This creates a 7-patch system consisting of a central Cartesian "core"
   * patch surrounded by 6 cubed-sphere wedge patches.
   * @note This is a host-only function that resets the object.
   */
  CCTK_HOST void select_cubedsphere(Index ncells, Coord xmin, Coord xmax,
                                    CCTK_REAL r0, CCTK_REAL r1) {
    *this = {};
    // A central Cartesian patch can fill the hole at the center of the sphere.
    add_patch(make_patch<CartesianMeta>(ncells, xmin, xmax));
    // Add the 6 surrounding wedge patches.
    for (auto w :
         {Wedge::PX, Wedge::NX, Wedge::PY, Wedge::NY, Wedge::PZ, Wedge::NZ})
      add_patch(make_patch<CubedSphereMeta>(ncells, xmin, xmax, w, r0, r1));
  }
};

//==============================================================================
// Global Multipatch Instance
//==============================================================================

/// @brief Defines the concrete type for the active multipatch system, setting
/// the maximum number of patches.
using AMP = MultiPatch<10>;

/// @brief The single, global instance of the active multipatch system.
///
/// It is declared as GPU-managed to be accessible from both host and device
/// code via unified memory.
AMREX_GPU_MANAGED AMP g_active_mp;

/**
 * @brief Provides convenient, inline access to the global multipatch system.
 */
inline AMP &active_mp() { return g_active_mp; }

} // namespace CurvBase

#endif // #ifndef CURVBASE_MULTIPATCH_HXX
