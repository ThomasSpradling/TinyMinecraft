#ifndef WORLD_H_
#define WORLD_H_

#include "Utils/mathgl.h"
#include "World/Chunk.h"
#include <unordered_set>
#include <vector>

namespace World {

class World {
public:
  World() = default;

  void Initialize();
  
  Chunk &GenerateChunk(glm::ivec2 &chunkPos);

  std::vector<glm::ivec2> GetNearbyChunks(glm::vec3 &pos, int radius);

  void Update(glm::vec3 &playerPos);

  std::unordered_map<glm::ivec2, Chunk, Utils::IVec2Hash> &GetChunks();

  bool IsFaceVisible(BlockFace face, const glm::vec3 &pos);

  void BreakBlock(const glm::vec3 &pos);
  void SetBlockAt(const glm::vec3 &pos, BlockType type);

  Block &GetBlockAt(const glm::vec3 &pos);
  
  bool HasBlock(const glm::vec3 &pos);
private:
  std::vector<glm::ivec2> chunkUpdateBuffer;
  std::unordered_set<glm::ivec2, Utils::IVec2Hash> visibleChunks;
  std::unordered_map<glm::ivec2, Chunk, Utils::IVec2Hash> chunks;

  glm::vec3 GetLocalBlockCoords(const glm::vec3 &pos);
  glm::ivec2 GetChunkPosFromCoords(const glm::vec3 &pos);

  int renderRadius = 20;
  int generateRadius = 22;

  const unsigned int seed = 123;
};

}

#endif // WORLD_H_
