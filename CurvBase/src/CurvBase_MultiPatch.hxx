
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

  CCTK_HOST CCTK_DEVICE const Patch *get_patch(std::size_t id) const noexcept {
    return (id < count_) ? &patches_[id] : nullptr;
  }

  CCTK_HOST CCTK_DEVICE Coord l2g(std::size_t id, const Coord &l) const {
    return (id < count_) ? patches_[id].map.l2g(l, patches_[id].meta_ptr())
                         : Coord{0, 0, 0};
  }

  CCTK_HOST CCTK_DEVICE Coord g2l(const Coord &g, std::size_t &id_out) const {
    for (std::size_t i = 0; i < count_; ++i) {
      const Coord loc = patches_[i].map.g2l(g, patches_[i].meta_ptr());
      if (patches_[i].map.is_valid(loc)) {
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

enum class MultiPatchMode { Cartesian, Spherical, CubedSphere };
using MP1 = MultiPatch<1>;
using MP7 = MultiPatch<7>;

struct ActiveMultiPatch {
  MultiPatchMode mode{MultiPatchMode::Cartesian};
  std::variant<MP1, MP7> mp{MP1{}}; // default = single patch

  CCTK_HOST CCTK_DEVICE std::size_t size() const noexcept {
    return std::visit([](auto const &m) { return m.size(); }, mp);
  }

  CCTK_HOST CCTK_DEVICE Coord l2g(std::size_t id, const Coord &l) const {
    return std::visit([&](auto const &m) { return m.l2g(id, l); }, mp);
  }

  CCTK_HOST CCTK_DEVICE Coord g2l(const Coord &g, std::size_t &id_out) const {
    return std::visit([&](auto const &m) { return m.g2l(g, id_out); }, mp);
  }

  // activate the 1-patch or 7-patch variant
  void select_cartesian() {
    mp.emplace<MP1>();
    mode = MultiPatchMode::Cartesian;
  }
  void select_spherical() {
    mp.emplace<MP1>();
    mode = MultiPatchMode::Spherical;
  }
  void select_cubedsphere() {
    mp.emplace<MP7>();
    mode = MultiPatchMode::CubedSphere;
  }

  // expose references if needed (host-only helpers, no device qualifier)
  MP1 &get_mp1() { return std::get<MP1>(mp); }
  MP7 &get_mp7() { return std::get<MP7>(mp); }
};

// Singleton access (thread-safe since C++11)
inline ActiveMultiPatch &active_mp() {
  static ActiveMultiPatch holder;
  return holder;
}
