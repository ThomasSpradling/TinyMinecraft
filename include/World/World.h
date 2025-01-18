#ifndef WORLD_H_
#define WORLD_H_

#include "World/Chunk.h"
#include <vector>

namespace World {

class World {
public:
  World() = default;
  ~World() = default;

  void Generate();
  void Update();

  std::vector<Chunk> &GetChunks();
private:
  std::vector<Chunk> chunks;
};

}

#endif // WORLD_H_
