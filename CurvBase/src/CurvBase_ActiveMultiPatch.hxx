enum class MultiPatchMode { Cartesian, Spherical, CubeSphere };

struct ActiveMultiPatch {
  PatchMode mode{PatchMode::Cartesian};

  union {
    MultiPatch<1> mp1;
    MultiPatch<7> mp7;
  };

  ActiveMultiPatch() : mp1{} {} // default Cartesian

  // uniform API
  CCTK_HOST CCTK_DEVICE std::size_t size() const {
    return (mode == PatchMode::CubeSphere) ? mp7.size() : mp1.size();
  }

  CCTK_HOST CCTK_DEVICE Coord local_to_global(std::size_t id,
                                              const Coord &l) const {
    return (mode == PatchMode::CubeSphere) ? mp7.local_to_global(id, l)
                                           : mp1.local_to_global(id, l);
  }

  CCTK_HOST CCTK_DEVICE Coord global_to_local(const Coord &g,
                                              std::size_t &out) const {
    return (mode == PatchMode::CubeSphere) ? mp7.global_to_local(g, out)
                                           : mp1.global_to_local(g, out);
  }
};

inline ActiveMultiPatch &active_mp() {
  static ActiveMultiPatch holder; // Thread-safe in C++11
  return holder;
}
