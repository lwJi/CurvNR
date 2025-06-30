
struct SphericalPatchData {
  double r_min, r_max;
};

__host__ __device__ Coord spherical_local_to_global(const Coord &local,
                                                    const void *data) {
  const auto *p = static_cast<const SphericalPatchData *>(data);
  double rho = local[0];   // ∈ [0, 1]
  double theta = local[1]; // ∈ [0, 2π]
  double phi = local[2];   // ∈ [0, π]
  double r = p->r_min + rho * (p->r_max - p->r_min);

  Coord global;
  global[0] = r * sin(phi) * cos(theta);
  global[1] = r * sin(phi) * sin(theta);
  global[2] = r * cos(phi);
  return global;
}

__host__ __device__ Coord spherical_global_to_local(const Coord &global,
                                                    const void *data) {
  const auto *p = static_cast<const SphericalPatchData *>(data);
  double x = global[0], y = global[1], z = global[2];
  double r = sqrt(x * x + y * y + z * z);
  double theta = atan2(y, x);
  double phi = acos(z / r);
  double rho = (r - p->r_min) / (p->r_max - p->r_min);
  return {rho, theta, phi};
}

struct CartesianPatchData {
  // Add grid bounds if needed
};

__host__ __device__ Coord cartesian_local_to_global(const Coord &local,
                                                    const void *data) {
  return local;
}

__host__ __device__ Coord cartesian_global_to_local(const Coord &global,
                                                    const void *data) {
  return global;
}

using Coord = std::array<double, 3>;

struct PatchFunctionSet {
  __host__ __device__ Coord (*local_to_global)(const Coord &, const void *);

  __host__ __device__ Coord (*global_to_local)(const Coord &, const void *);
};

struct Patch {
  PatchFunctionSet functions;
  const void *userdata;
};

template <int MaxPatches> struct MultiPatch {
  Patch patches[MaxPatches];
  int num_patches;

  __host__ __device__ Coord local_to_global(int pid, const Coord &local) const {
    return patches[pid].functions.local_to_global(local, patches[pid].userdata);
  }

  __host__ __device__ Coord global_to_local(const Coord &global,
                                            int &out_patch_id) const {
    for (int i = 0; i < num_patches; ++i) {
      Coord local =
          patches[i].functions.global_to_local(global, patches[i].userdata);
      if (is_valid_local(local)) {
        out_patch_id = i;
        return local;
      }
    }
    out_patch_id = -1;
    return {0.0, 0.0, 0.0};
  }

  __host__ __device__ static bool is_valid_local(const Coord &local) {
    for (int i = 0; i < 3; ++i)
      if (local[i] < -1.1 || local[i] > 1.1)
        return false;
    return true;
  }

  __host__ void add_patch(int pid, const Patch &p) { patches[pid] = p; }
};

MultiPatch<1> make_cartesian_patch() {
  static CartesianPatchData *data = new CartesianPatchData{};
  Patch patch{{cartesian_local_to_global, cartesian_global_to_local}, data};
  MultiPatch<1> mp;
  mp.add_patch(0, patch);
  mp.num_patches = 1;
  return mp;
}

MultiPatch<1> make_spherical_patch(double r_min, double r_max) {
  static SphericalPatchData *data = new SphericalPatchData{r_min, r_max};
  Patch patch{{spherical_local_to_global, spherical_global_to_local}, data};
  MultiPatch<1> mp;
  mp.add_patch(0, patch);
  mp.num_patches = 1;
  return mp;
}

MultiPatch<7> make_multipatch_cube_sphere() {
  MultiPatch<7> mp;

  static CenterCartesianData center_data{};
  mp.add_patch(
      0, Patch{{cart_local_to_global, cart_global_to_local}, &center_data});

  static CubedSphereWedgeData wedge_data[6] = {
      {Face::PX, 1.0, 2.0}, {Face::NX, 1.0, 2.0}, {Face::PY, 1.0, 2.0},
      {Face::NY, 1.0, 2.0}, {Face::PZ, 1.0, 2.0}, {Face::NZ, 1.0, 2.0},
  };

  for (int i = 0; i < 6; ++i) {
    mp.add_patch(i + 1, Patch{{wedge_local_to_global, wedge_global_to_local},
                              &wedge_data[i]});
  }

  mp.num_patches = 7;
  return mp;
}

auto mp1 = make_cartesian_patch();
Coord g1 = mp1.local_to_global(0, {0.1, 0.2, 0.3});

auto mp2 = make_spherical_patch(1.0, 2.0);
Coord g2 = mp2.local_to_global(0, {0.5, M_PI / 4, M_PI / 3});

auto mp3 = make_multipatch_cube_sphere();
Coord g3 = mp3.local_to_global(2, {0.0, 0.0, 0.5}); // Use a wedge
