#include "Math/seed.h"

namespace TinyMinecraft {

  auto TwistSeedLCG(int seed) -> int {
    unsigned int x = static_cast<unsigned int>(seed);
    x = 1664525u * x + 1013904223u;
    return static_cast<int>(x);
  }

  auto TwistSeedFNV1a(int seed) -> int
  {
    const unsigned int FNV_offset_basis = 2166136261u;
    const unsigned int FNV_prime        = 16777619u;

    unsigned int x = FNV_offset_basis ^ static_cast<unsigned int>(seed);
    x *= FNV_prime;
    return static_cast<int>(x);
  }

}

