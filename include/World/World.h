#ifndef WORLD_H_
#define WORLD_H_

#include "Utils/mathgl.h"
#include "World/Chunk.h"
#include <vector>

namespace World {

class World {
public:
  World() = default;
  ~World() = default;

  void Generate();
  void Update();

  // // In chunk coordinates
  // Chunk &GetChunkAt(int x, int z);
  // Chunk &GetChunkAt(const glm::ivec2 &pos);

  // // In world coordinates
  // Block &GetBlockAt(int x, int y, int z);
  // Block &GetBlockAt(const glm::vec3 &pos);

  glm::vec3 GetLocalBlockCoords(const glm::vec3 &pos);
  glm::ivec2 GetChunkPosFromCoords(const glm::vec3 &pos);

  std::unordered_map<glm::ivec2, Chunk, Utils::IVec2Hash> &GetChunks();

  bool IsFaceVisible(BlockFace face, const glm::vec3 &pos);
private:
  std::unordered_map<glm::ivec2, Chunk, Utils::IVec2Hash> chunks;

};

}

#endif // WORLD_H_
