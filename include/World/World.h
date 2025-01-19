#ifndef WORLD_H_
#define WORLD_H_

#include "Utils/mathgl.h"
#include "World/Chunk.h"
#include <queue>
#include <unordered_set>
#include <vector>

namespace World {

class World {
public:
  World() = default;
  ~World() = default;

  void Generate();
  Chunk &GenerateChunk(glm::ivec2 &offset);

  std::vector<glm::ivec2> GetNearbyChunks(glm::vec3 &pos, int radius);

  void Update(glm::vec3 &playerPos);
  void UpdateChunk(Chunk &chunk);

  std::unordered_map<glm::ivec2, Chunk, Utils::IVec2Hash> &GetChunks();

  bool IsFaceVisible(BlockFace face, const glm::vec3 &pos);
private:
  std::deque<glm::ivec2> chunkUpdateBuffer;
  std::unordered_set<glm::ivec2, Utils::IVec2Hash> loadedChunks;
  std::unordered_set<glm::ivec2, Utils::IVec2Hash> visibleChunks;
  std::unordered_map<glm::ivec2, Chunk, Utils::IVec2Hash> chunks;

  glm::vec3 GetLocalBlockCoords(const glm::vec3 &pos);
  glm::ivec2 GetChunkPosFromCoords(const glm::vec3 &pos);

  // bool IsChunkNearby(glm::vec3 &pos, glm::ivec2 chunkPos);

  int renderRadius = 5;
  int generateRadius = 10;
};

}

#endif // WORLD_H_
