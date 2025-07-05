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
  CCTK_HOST CCTK_DEVICE MultiPatch() = default;

  CCTK_HOST CCTK_DEVICE std::size_t add_patch(const Patch &p) {
    return (count_ < MaxP) ? (patches_[count_] = p, count_++) : MaxP;
  }

  CCTK_HOST CCTK_DEVICE const Patch *get_patch(std::size_t id) const noexcept {
    return (id < count_) ? &patches_[id] : nullptr;
  }

  CCTK_HOST CCTK_DEVICE Coord l2g(std::size_t id, const Coord &l) const {
    return (id < count_) ? patches_[id].l2g(l) : Coord{0, 0, 0};
  }

  CCTK_HOST CCTK_DEVICE Coord g2l(const Coord &g, std::size_t &id_out) const {
    for (std::size_t i = 0; i < count_; ++i) {
      const Coord loc = patches_[i].g2l(g);
      if (patches_[i].is_valid(loc)) {
        id_out = i;
        return loc;
      }
    }
    id_out = static_cast<std::size_t>(-1);
    return {0, 0, 0};
  }

  CCTK_HOST CCTK_DEVICE std::size_t size() const { return count_; }
};

//------------------------------------------------------------------------------
// Runtime-selectable patch system
//------------------------------------------------------------------------------

template <std::size_t MaxP> struct ActiveMultiPatch {
  MultiPatch<MaxP> mp; // just one concrete object

  // thin wrappers forward to mp
  CCTK_HOST CCTK_DEVICE std::size_t size() const { return mp.size(); }

  CCTK_HOST CCTK_DEVICE const Patch *get_patch(std::size_t id) const {
    return mp.get_patch(id);
  }

  CCTK_HOST CCTK_DEVICE Coord l2g(std::size_t id, Coord const &l) const {
    return mp.l2g(id, l);
  }

  CCTK_HOST CCTK_DEVICE Coord g2l(Coord const &g, std::size_t &idp) const {
    return mp.g2l(g, idp);
  }

  // helpers that *build* the patch set on the host
  void select_cartesian() {
    mp = {};
    mp.add_patch(make_cart_patch());
  }

  void select_spherical(const CCTK_REAL r0, const CCTK_REAL r1) {
    mp = {};
    mp.add_patch(make_sph_patch(r0, r1));
  }

  void select_cubedsphere(const CCTK_REAL r0, const CCTK_REAL r1) {
    mp = {};
    mp.add_patch(make_cart_patch()); // core
    mp.add_patch(make_wedge_patch(Face::PX, r0, r1));
    mp.add_patch(make_wedge_patch(Face::NX, r0, r1));
    mp.add_patch(make_wedge_patch(Face::PY, r0, r1));
    mp.add_patch(make_wedge_patch(Face::NY, r0, r1));
    mp.add_patch(make_wedge_patch(Face::PZ, r0, r1));
    mp.add_patch(make_wedge_patch(Face::NZ, r0, r1));
  }
};

//------------------------------------------------------------------------------
// Live in unified memory
//------------------------------------------------------------------------------

using AMP = ActiveMultiPatch<10>; // maximum 10 patches

AMREX_GPU_MANAGED AMP g_active_mp;

inline AMP &active_mp() { return g_active_mp; }

} // namespace CurvBase

#endif // #ifndef CURVBASE_MULTIPATCH_HXX
