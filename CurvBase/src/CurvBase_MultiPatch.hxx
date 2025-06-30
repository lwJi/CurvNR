
struct PatchMap {
  CCTK_HOST CCTK_DEVICE Coord (*local_to_global)(const Coord &,
                                                 const void *) = nullptr;
  CCTK_HOST CCTK_DEVICE Coord (*global_to_local)(const Coord &,
                                                 const void *) = nullptr;
};

struct Patch {
  PatchMap map;
  void *meta = nullptr;
};

template <std::size_t N> class MultiPatch {
public:
  CCTK_HOST CCTK_DEVICE MultiPatch() : count_(0) {} // empty by default

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
      if (is_valid_local(loc)) {
        id_out = i;
        return loc;
      }
    }
    id_out = static_cast<std::size_t>(-1);
    return {0, 0, 0};
  }

  CCTK_HOST CCTK_DEVICE std::size_t size() const { return count_; }

private:
  Patch patches_[N];
  std::size_t count_;

  CCTK_HOST CCTK_DEVICE static constexpr bool is_valid_local(const Coord &l) {
    return (l[0] >= -1.1 && l[0] <= 1.1) && (l[1] >= -1.1 && l[1] <= 1.1) &&
           (l[2] >= -1.1 && l[2] <= 1.1);
  }
};
