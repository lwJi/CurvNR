
struct CartesianMeta { /* add bounds if you wish */
};

CCTK_HOST CCTK_DEVICE inline Coord cart_local_to_global(const Coord &l,
                                                        const void *) {
  return l;
}
CCTK_HOST CCTK_DEVICE inline Coord cart_global_to_local(const Coord &g,
                                                        const void *) {
  return g;
}
