#ifndef CURVBASE_MULTIPATCH_HXX
#define CURVBASE_MULTIPATCH_HXX

#include <AMReX_Gpu.H>

#include <variant>

#include "CurvBase_Patch.hxx"

namespace CurvBase {

//------------------------------------------------------------------------------
// Container for a fixed maximum number of patches
//------------------------------------------------------------------------------

template <std::size_t MaxP> class MultiPatch {
  Patch patches_[MaxP]{};
  std::size_t count_{0};

public:
  CCTK_HOST MultiPatch() = default;

  CCTK_HOST std::size_t add_patch(const Patch &p) {
    return (count_ < MaxP) ? (patches_[count_] = p, count_++) : MaxP;
  }

  CCTK_HOST CCTK_DEVICE const Patch *get_patch(std::size_t id) const noexcept {
    return (id < count_) ? &patches_[id] : nullptr;
  }

  CCTK_HOST CCTK_DEVICE Coord l2g(std::size_t id,
                                  const Coord &l) const noexcept {
    return (id < count_) ? patches_[id].l2g(l) : Coord{0, 0, 0};
  }

  CCTK_HOST CCTK_DEVICE std::pair<Coord, std::size_t>
  g2l(const Coord &g) const noexcept {
    for (std::size_t i = 0; i < count_; ++i) {
      const Coord loc = patches_[i].g2l(g);
      if (patches_[i].is_valid(loc)) {
        return {loc, i};
      }
    }
    return {Coord{}, -1};
  }

  CCTK_HOST CCTK_DEVICE std::size_t size() const noexcept { return count_; }

  // HOST-ONLY BUILDERS (never called from device)
  CCTK_HOST void select_cartesian(Index ncells, Coord xmin, Coord xmax) {
    *this = {};
    add_patch(make_patch<CartesianMeta>(ncells, xmin, xmax));
  }

  CCTK_HOST void select_spherical(Index ncells, Coord xmin, Coord xmax) {
    *this = {};
    add_patch(make_patch<SphericalMeta>(ncells, xmin, xmax));
  }

  CCTK_HOST void select_cubedsphere(Index ncells, Coord xmin, Coord xmax,
                                    CCTK_REAL r0, CCTK_REAL r1) {
    *this = {};
    add_patch(make_patch<CartesianMeta>(ncells, xmin, xmax)); // core
    for (auto w :
         {Wedge::PX, Wedge::NX, Wedge::PY, Wedge::NY, Wedge::PZ, Wedge::NZ})
      add_patch(make_patch<CubedSphereMeta>(ncells, xmin, xmax, w, r0, r1));
  }
};

//------------------------------------------------------------------------------
// Live in unified memory
//------------------------------------------------------------------------------

using AMP = MultiPatch<10>; // maximum 10 patches

AMREX_GPU_MANAGED AMP g_active_mp;

inline AMP &active_mp() { return g_active_mp; }

} // namespace CurvBase

#endif // #ifndef CURVBASE_MULTIPATCH_HXX
