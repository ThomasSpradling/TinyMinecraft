#ifndef SEED_H_
#define SEED_H_

namespace TinyMinecraft {

  inline auto TwistSeedLCG(int seed) -> int {
    unsigned int x = static_cast<unsigned int>(seed);
    x = 1664525u * x + 1013904223u;
    return static_cast<int>(x);
  }

  inline auto TwistSeedFNV1a(int seed) -> int
  {
    const unsigned int FNV_offset_basis = 2166136261u;
    const unsigned int FNV_prime        = 16777619u;

    unsigned int x = FNV_offset_basis ^ static_cast<unsigned int>(seed);
    x *= FNV_prime;
    return static_cast<int>(x);
  }

}

#endif // SEED_H_


