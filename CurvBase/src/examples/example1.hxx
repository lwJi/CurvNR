
struct CartesianMeta {};

CCTK_HOST CCTK_DEVICE inline Coord cart_local_to_global(const Coord &l,
                                                        const void *) {
  return l;
}

CCTK_HOST CCTK_DEVICE inline Coord cart_global_to_local(const Coord &g,
                                                        const void *) {
  return g;
}

CCTK_HOST CCTK_DEVICE inline bool cart_valid(const Coord &) { return true; }

enum class Face { PX, NX, PY, NY, PZ, NZ };

struct CubedSphereMeta {
  Face face;               // which cube face
  double r_inner, r_outer; // radial extent of the wedge
};

CCTK_HOST CCTK_DEVICE inline Coord cubedsphere_local_to_global(const Coord &l,
                                                               const void *m) {
  const auto *p = static_cast<const CubedSphereMeta *>(m);
  const double xi = l[0], eta = l[1], rho = l[2];
  const double r = p->r_inner + rho * (p->r_outer - p->r_inner);
  const double d = std::sqrt(1.0 + xi * xi + eta * eta);

  switch (p->face) {
  case Face::PX:
    return {r / d, r * xi / d, r * eta / d};
  case Face::NX:
    return {-r / d, -r * xi / d, r * eta / d};
  case Face::PY:
    return {-r * xi / d, r / d, r * eta / d};
  case Face::NY:
    return {r * xi / d, -r / d, r * eta / d};
  case Face::PZ:
    return {r * xi / d, r * eta / d, r / d};
  case Face::NZ:
    return {r * xi / d, -r * eta / d, -r / d};
  }
  return {0, 0, 0}; // unreachable
}

CCTK_HOST CCTK_DEVICE inline Coord cubedsphere_global_to_local(const Coord &g,
                                                               const void *m) {
  const auto *p = static_cast<const CubedSphereMeta *>(m);
  const double x = g[0], y = g[1], z = g[2];
  const double r = std::sqrt(sq(x) + sq(y) + sq(z));

  double xi = 0, eta = 0;
  switch (p->face) {
  case Face::PX:
    xi = y / x;
    eta = z / x;
    break;
  case Face::NX:
    xi = -y / x;
    eta = z / x;
    break;
  case Face::PY:
    xi = -x / y;
    eta = z / y;
    break;
  case Face::NY:
    xi = x / y;
    eta = z / y;
    break;
  case Face::PZ:
    xi = x / z;
    eta = y / z;
    break;
  case Face::NZ:
    xi = x / z;
    eta = -y / z;
    break;
  }
  const double rho = (r - p->r_inner) / (p->r_outer - p->r_inner);
  return {xi, eta, rho};
}

CCTK_HOST CCTK_DEVICE inline bool cubedsphere_valid(const Coord &l) {
  return (l[0] >= -1 && l[0] <= 1) && (l[1] >= -1 && l[1] <= 1) &&
         (l[2] >= 0 && l[2] <= 1);
}

struct SphericalMeta {
  double r_min, r_max; // inner / outer radius
};

CCTK_HOST CCTK_DEVICE inline Coord sph_local_to_global(const Coord &l,
                                                       const void *m) {
  const auto *p = static_cast<const SphericalMeta *>(m);
  const double rho = l[0];   // [0,1]
  const double theta = l[1]; // [0,2π]
  const double phi = l[2];   // [0,π]
  const double r = p->r_min + rho * (p->r_max - p->r_min);
  return {r * std::sin(phi) * std::cos(theta),
          r * std::sin(phi) * std::sin(theta), r * std::cos(phi)};
}

CCTK_HOST CCTK_DEVICE inline Coord sph_global_to_local(const Coord &g,
                                                       const void *m) {
  const auto *p = static_cast<const SphericalMeta *>(m);
  const double x = g[0], y = g[1], z = g[2];
  const double r = std::sqrt(sq(x) + sq(y) + sq(z));
  return {(r - p->r_min) / (p->r_max - p->r_min), // ρ
          std::atan2(y, x),                       // theta
          std::acos(z / r)};                      // phi
}

CCTK_HOST CCTK_DEVICE inline bool sph_valid(const Coord &l) {
  return (l[0] >= 0 && l[0] <= 1) && (l[1] >= 0 && l[1] <= 2 * M_PI) &&
         (l[2] >= 0 && l[2] <= M_PI);
}

struct PatchMap {
  CCTK_HOST CCTK_DEVICE Coord (*local_to_global)(const Coord &,
                                                 const void *) = nullptr;
  CCTK_HOST CCTK_DEVICE Coord (*global_to_local)(const Coord &,
                                                 const void *) = nullptr;
  CCTK_HOST CCTK_DEVICE bool (*is_valid_local)(const Coord &) = nullptr;
};

struct FaceInfo {
  bool is_outer_boundary{true};
};

struct Patch {
  // metadata container
  std::variant<CartesianMeta, SphericalMeta, CubedSphereMeta> meta{};

  // helper: return pointer to the **concrete** meta inside the variant
  CCTK_HOST CCTK_DEVICE const void *meta_ptr() const noexcept {
    return std::visit([](auto const &m) -> const void * { return &m; }, meta);
  }

  PatchMap map{};

  std::array<CCTK_INT, dim> ncells{}; // cells per dimension
  std::array<CCTK_REAL, dim> xmin{};  // lower physical bounds
  std::array<CCTK_REAL, dim> xmax{};  // upper physical bounds

  std::array<std::array<FaceInfo, dim>, 2> faces{};
};

//------------------------------------------------------------------------------
// Helpers that create individual Patch objects
//------------------------------------------------------------------------------
inline Patch make_cart_patch() {
  Patch p;
  p.map = {cart_local_to_global, cart_global_to_local, cart_valid};
  p.meta = CartesianMeta{}; // active alt set
  return p;
}

inline Patch make_sph_patch(CCTK_REAL r0, CCTK_REAL r1) {
  Patch p;
  p.map = {sph_local_to_global, sph_global_to_local, sph_valid};
  p.meta = SphericalMeta{r0, r1};
  return p;
}

inline Patch make_wedge(Face f, CCTK_REAL r0, CCTK_REAL r1) {
  Patch p;
  p.map = {cubedsphere_local_to_global, cubedsphere_global_to_local,
           cubedsphere_valid};
  p.meta = CubedSphereMeta{f, r0, r1};
  return p;
}

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

  CCTK_HOST CCTK_DEVICE Coord local_to_global(std::size_t id,
                                              const Coord &l) const {
    return (id < count_)
               ? patches_[id].map.local_to_global(l, patches_[id].meta_ptr())
               : Coord{0, 0, 0};
  }

  CCTK_HOST CCTK_DEVICE Coord global_to_local(const Coord &g,
                                              std::size_t &id_out) const {
    for (std::size_t i = 0; i < count_; ++i) {
      const Coord loc =
          patches_[i].map.global_to_local(g, patches_[i].meta_ptr());
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

enum class MultiPatchMode { Cartesian, Spherical, CubedSphere };

struct ActiveMultiPatch {
  MultiPatchMode mode{MultiPatchMode::Cartesian};

  union {
    MultiPatch<1> mp1;
    MultiPatch<7> mp7;
  };

  ActiveMultiPatch() : mp1{} {} // default single patch

  CCTK_HOST CCTK_DEVICE std::size_t size() const {
    return (mode == MultiPatchMode::CubedSphere) ? mp7.size() : mp1.size();
  }

  CCTK_HOST CCTK_DEVICE Coord local_to_global(std::size_t id,
                                              const Coord &l) const {
    return (mode == MultiPatchMode::CubedSphere) ? mp7.local_to_global(id, l)
                                                 : mp1.local_to_global(id, l);
  }

  CCTK_HOST CCTK_DEVICE Coord global_to_local(const Coord &g,
                                              std::size_t &id_out) const {
    return (mode == MultiPatchMode::CubedSphere)
               ? mp7.global_to_local(g, id_out)
               : mp1.global_to_local(g, id_out);
  }
};

// Singleton access (thread-safe since C++11)
inline ActiveMultiPatch &active_mp() {
  static ActiveMultiPatch holder;
  return holder;
}

extern "C" void CurvBase_MultiPatch_Setup() {
  DECLARE_CCTK_PARAMETERS;

  ActiveMultiPatch tmp;
  tmp.mode = mode;

  switch (mode) {
  case MultiPatchMode::Cartesian:
    tmp.mp1.add_patch(make_cart_patch());
    break;

  case MultiPatchMode::Spherical:
    tmp.mp1.add_patch(make_sph_patch(p.r_min, p.r_max));
    break;

  case MultiPatchMode::CubedSphere:
    // CubedSphere: 6 wedges + central Cartesian at the end
    tmp.mp7.add_patch(make_wedge(Face::PX, p.r_min, p.r_max)); // id 0
    tmp.mp7.add_patch(make_wedge(Face::NX, p.r_min, p.r_max)); // id 1
    tmp.mp7.add_patch(make_wedge(Face::PY, p.r_min, p.r_max)); // id 2
    tmp.mp7.add_patch(make_wedge(Face::NY, p.r_min, p.r_max)); // id 3
    tmp.mp7.add_patch(make_wedge(Face::PZ, p.r_min, p.r_max)); // id 4
    tmp.mp7.add_patch(make_wedge(Face::NZ, p.r_min, p.r_max)); // id 5
    tmp.mp7.add_patch(make_cart_patch());                      // id 6 (core)
    break;

  default:
    CCTK_VERROR("Unknown multi-patch system \"%s\"", patch_system);
  }

  active_mp() = tmp; // move into the global store
}

extern "C" void CurvBase_MultiPatch_Setup_Coordinates() {
  DECLARE_CCTK_PARAMETERS;

  // access active multipatch system
  auto &mp = active_mp();
}

//------------------------------------------------------------------------------
// Interfaces with the CarpetX driver
//------------------------------------------------------------------------------

extern "C" CCTK_INT
CurvBase_MultiPatch_GetSystemSpecification(CCTK_INT *restrict const npatches) {
  *npatches = *npatches = static_cast<CCTK_INT>(active_mp().size());
  return 0;
}

extern "C" CCTK_INT CurvBase_MultiPatch_GetPatchSpecification(
    const CCTK_INT ipatch, CCTK_INT *restrict const is_cartesian,
    const CCTK_INT size, CCTK_INT *restrict const ncells,
    CCTK_REAL *restrict const xmin, CCTK_REAL *restrict const xmax) {
  auto &mp = active_mp();

  assert(size == dim);
  assert(ipatch >= 0 && ipatch < static_cast<CCTK_INT>(mp.size()));

  const Patch *patch = mp.get_patch(static_cast<std::size_t>(ipatch));
  assert(patch != nullptr);

  if (is_cartesian) {
    *is_cartesian = patch->is_cartesian ? 1 : 0;
  }

  for (int d = 0; d < dim; ++d) {
    ncells[d] = patch->ncells[d];
    xmin[d] = patch->xmin[d];
    xmax[d] = patch->xmax[d];
  }

  return 0;
}

extern "C" CCTK_INT CurvBase_MultiPatch_GetBoundarySpecification(
    const CCTK_INT ipatch, const CCTK_INT size,
    CCTK_INT *restrict const is_interpatch_boundary) {
  auto &mp = active_mp();

  assert(size == 2 * dim);
  assert(ipatch >= 0 && ipatch < static_cast<CCTK_INT>(mp.size()));

  const Patch *patch = mp.get_patch(static_cast<std::size_t>(ipatch));
  assert(patch != nullptr);

  for (int d = 0; d < dim; ++d) {
    is_interpatch_boundary[2 * d + 0] =
        patch->faces[0][d].is_outer_boundary ? 0 : 1;
    is_interpatch_boundary[2 * d + 1] =
        patch->faces[1][d].is_outer_boundary ? 0 : 1;
  }

  return 0;
}
