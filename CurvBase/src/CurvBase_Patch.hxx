#ifndef CURVBASE_PATCH_HXX
#define CURVBASE_PATCH_HXX

#include <array>
#include <cmath>

namespace CurvBase {

using Coord = std::array<double, 3>;

// Cartesian patch: identity transform
struct Cartesian {
  CCTK_HOST CCTK_DEVICE static Coord local_to_global(const Coord &local) {
    return local; // No transform
  }

  CCTK_HOST CCTK_DEVICE static Coord global_to_local(const Coord &global) {
    return global; // No transform
  }
};

// Spherical patch: example transform to Cartesian coords
struct Spherical {
  CCTK_HOST CCTK_DEVICE static Coord local_to_global(const Coord &local) {
    Coord result;
    double r = local[0];
    double theta = local[1];
    double phi = local[2];

    result[0] = r * sin(phi) * cos(theta); // x
    result[1] = r * sin(phi) * sin(theta); // y
    result[2] = r * cos(phi);              // z
    return result;
  }

  CCTK_HOST CCTK_DEVICE static Coord global_to_local(const Coord &global) {
    Coord result;
    double x = global[0], y = global[1], z = global[2];
    double r = sqrt(x * x + y * y + z * z);
    double theta = atan2(y, x);
    double phi = acos(z / r);
    result[0] = r;
    result[1] = theta;
    result[2] = phi;
    return result;
  }
};

template <typename PatchType>
CCTK_HOST CCTK_DEVICE Coord call_local_to_global(const Coord &local) {
  return PatchType::local_to_global(local);
}

template <typename PatchType>
CCTK_HOST CCTK_DEVICE Coord call_global_to_local(const Coord &global) {
  return PatchType::global_to_local(global);
}

} // namespace CurvBase

#endif // #ifndef CURVBASE_PATCH_HXX
