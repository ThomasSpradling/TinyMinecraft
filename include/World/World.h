#ifndef WORLD_H_
#define WORLD_H_

#include "Utils/mathgl.h"
#include "Utils/noise.h"
#include "World/Chunk.h"
#include "World/WorldGeneration.h"
#include <unordered_set>
#include <vector>

namespace World {

class World {
public:
  World();

  void Initialize();
  
  void GenerateChunk(const glm::ivec2 &chunkPos);

  std::vector<glm::ivec2> GetNearbyChunks(const glm::vec3 &pos, int radius);
  Chunk &CreateEmptyChunk(const glm::ivec2 &chunkPos);

  float GetTemperature(int x, int z);
  float GetHumidity(int x, int z);
  BiomeType GetBiome(int x, int z);

  void Update(const glm::vec3 &playerPos);

  void UnloadChunk(int x, int z);
  void UnloadChunk(Chunk &chunk);
  void LoadChunk(int x, int z);
  void LoadChunk(Chunk &chunk);

  std::unordered_map<glm::ivec2, Chunk, Utils::IVec2Hash> &GetLoadedChunks();

  bool IsFaceVisible(BlockFace face, const glm::vec3 &pos);

  void BreakBlock(const glm::vec3 &pos);
  void SetBlockAt(const glm::vec3 &pos, BlockType type);

  Block &GetBlockAt(const glm::vec3 &pos);
  bool HasBlock(const glm::vec3 &pos);

  Chunk &GetChunkAt(int x, int z);
  Chunk &GetChunkAt(const glm::ivec2 &chunkPos);
  
  glm::ivec2 GetChunkPosFromCoords(const glm::vec3 &pos);
private:
  const unsigned int seed = 3782;

  PerlinNoise temperatureMap, humidityMap, blendMap, heightMap, stoneMap;
  
  // std::vector<glm::ivec2> chunkUpdateBuffer;
  // std::unordered_set<glm::ivec2, Utils::IVec2Hash> visibleChunks;
  // std::vector<Chunk> loadedChunks;
  std::unordered_map<glm::ivec2, Chunk, Utils::IVec2Hash> loadedChunks;

  glm::vec3 GetLocalBlockCoords(const glm::vec3 &pos);

  WorldGeneration worldGen;

  // int renderRadius = i_GFX_RenderDistance;
  // int generateRadius = i_GFX_RenderDistance + 2;

};

}

#endif // WORLD_H_
