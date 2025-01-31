#ifndef WORLD_H_
#define WORLD_H_

#include "Geometry/geometry.h"
#include "Utils/mathgl.h"
#include "World/Chunk.h"
#include "World/WorldGeneration.h"

namespace World {

class World {
public:
  World();

  auto GetTemperature(int x, int z) -> double;
  auto GetHumidity(int x, int z) -> double;
  auto GetContinentalness(int x, int z) -> double;
  auto GetErosion(int x, int z) -> double;

  auto GetBiome(int x, int z) -> BiomeType;

  void Update(const glm::vec3 &playerPos);

  void UnloadChunk(int x, int z);
  void UnloadChunk(Chunk &chunk);
  void LoadChunk(int x, int z);
  void LoadChunk(Chunk &chunk);

  auto IsChunkLoaded(const glm::ivec2 &pos) -> bool;

  auto GetLoadedChunks() -> std::unordered_map<glm::ivec2, Chunk, Utils::IVec2Hash> &;

  auto IsFaceVisible(const Block &block, Geometry::Face face, const glm::vec3 &pos) -> bool;

  void BreakBlock(const glm::vec3 &pos);
  void SetBlockAt(const glm::vec3 &pos, BlockType type);
  
  auto GetBlockAt(const glm::vec3 &pos) -> Block &;
  auto HasBlock(const glm::vec3 &pos) -> bool;

  auto GetChunkAt(int x, int z) -> Chunk &;
  auto GetChunkAt(const glm::ivec2 &chunkPos) -> Chunk &;
  
  auto GetChunkPosFromCoords(const glm::vec3 &pos) -> glm::ivec2;
  auto GetLocalBlockCoords(const glm::vec3 &pos) -> glm::vec3;

  void HandlePlayerMovement(const glm::vec3 &before, const glm::vec3 &after);

private:
  const unsigned int seed = 3782;

  // PerlinNoise m_temperatureMap, m_humidityMap, m_blendMap, m_heightMap, m_stoneMap, m_treeMap, m_grassMap, m_continentalnessMap, m_erosionMap;
  
  std::unordered_map<glm::ivec2, Chunk, Utils::IVec2Hash> m_loadedChunks;
  WorldGeneration m_worldGen;
};

}

#endif // WORLD_H_
