#ifndef SEED_H_
#define SEED_H_

namespace TinyMinecraft {

  [[nodiscard]] auto TwistSeedLCG(int seed) -> int;
  [[nodiscard]] auto TwistSeedFNV1a(int seed) -> int;

}

#endif // SEED_H_


