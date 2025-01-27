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

  float GetTemperature(int x, int z);
  float GetHumidity(int x, int z);
  BiomeType GetBiome(int x, int z);

  void Update(const glm::vec3 &playerPos);
  void Render(Graphics::Texture &blockAtlasTexture, GLuint depthMap, Graphics::Shader &blockShader, Graphics::Shader &waterShader, const glm::vec3 &playerPos);

  void UnloadChunk(int x, int z);
  void UnloadChunk(Chunk &chunk);
  void LoadChunk(int x, int z);
  void LoadChunk(Chunk &chunk);

  bool IsChunkLoaded(const glm::ivec2 &pos);

  std::unordered_map<glm::ivec2, Chunk, Utils::IVec2Hash> &GetLoadedChunks();

  bool IsFaceVisible(const Block &block, BlockFace face, const glm::vec3 &pos);

  void BreakBlock(const glm::vec3 &pos);
  void SetBlockAt(const glm::vec3 &pos, BlockType type);
  
  Block &GetBlockAt(const glm::vec3 &pos);
  bool HasBlock(const glm::vec3 &pos);

  Chunk &GetChunkAt(int x, int z);
  Chunk &GetChunkAt(const glm::ivec2 &chunkPos);
  
  glm::ivec2 GetChunkPosFromCoords(const glm::vec3 &pos);
  glm::vec3 GetLocalBlockCoords(const glm::vec3 &pos);

  void HandlePlayerMovement(const glm::vec3 &before, const glm::vec3 &after);
private:
  const unsigned int seed = 3782;

  PerlinNoise temperatureMap, humidityMap, blendMap, heightMap, stoneMap, treeMap, grassMap;
  
  std::unordered_map<glm::ivec2, Chunk, Utils::IVec2Hash> loadedChunks;
  WorldGeneration worldGen;


};

}

#endif // WORLD_H_
