
//------------------------------------------------------------------------------
// Container for a fixed maximum number of patches
//------------------------------------------------------------------------------

template <std::size_t N> class MultiPatch {
  Patch patches_[N]{};
  std::size_t count_{0};

public:
  CCTK_HOST CCTK_DEVICE MultiPatch() = default;

  CCTK_HOST CCTK_DEVICE std::size_t add_patch(const Patch &p) {
    return (count_ < N) ? (patches_[count_] = p, count_++) : N;
  }

  CCTK_HOST CCTK_DEVICE Coord local_to_global(std::size_t id,
                                              const Coord &l) const {
    return (id < count_)
               ? patches_[id].map.local_to_global(l, patches_[id].meta)
               : Coord{0, 0, 0};
  }

  CCTK_HOST CCTK_DEVICE Coord global_to_local(const Coord &g,
                                              std::size_t &id_out) const {
    for (std::size_t i = 0; i < count_; ++i) {
      const Coord loc = patches_[i].map.global_to_local(g, patches_[i].meta);
      if (patches_[i].map.is_valid_local(loc)) {
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

enum class MultiPatchMode { Cartesian, Spherical, CubeSphere };

struct ActiveMultiPatch {
  MultiPatchMode mode{MultiPatchMode::Cartesian};

  union {
    MultiPatch<1> mp1;
    MultiPatch<7> mp7;
  };

  ActiveMultiPatch() : mp1{} {} // default single patch

  CCTK_HOST CCTK_DEVICE std::size_t size() const {
    return (mode == MultiPatchMode::CubeSphere) ? mp7.size() : mp1.size();
  }

  CCTK_HOST CCTK_DEVICE Coord local_to_global(std::size_t id,
                                              const Coord &l) const {
    return (mode == MultiPatchMode::CubeSphere) ? mp7.local_to_global(id, l)
                                                : mp1.local_to_global(id, l);
  }

  CCTK_HOST CCTK_DEVICE Coord global_to_local(const Coord &g,
                                              std::size_t &id_out) const {
    return (mode == MultiPatchMode::CubeSphere)
               ? mp7.global_to_local(g, id_out)
               : mp1.global_to_local(g, id_out);
  }
};

// Singleton access (thread-safe since C++11)
inline ActiveMultiPatch &active_mp() {
  static ActiveMultiPatch holder;
  return holder;
}
