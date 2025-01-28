#ifndef WORLD_GENERATION_H_
#define WORLD_GENERATION_H_

#include "Utils/noise.h"
#include "Utils/mathgl.h"
#include "World/Biome.h"
#include "World/Block.h"
#include "World/Chunk.h"

namespace World {

class World;

class WorldGeneration {
public:
  WorldGeneration(World &world);
  void GenerateTerrainChunk(Chunk &chunk, PerlinNoise &blendMap, PerlinNoise &heightMap, PerlinNoise &stoneMap);
  void GenerateFeatures(Chunk &chunk, PerlinNoise &treeMap, PerlinNoise &grassMap);

  void LoadUnloadedBlocks(Chunk &chunk);

  std::pair <const Biome*, const Biome*> SelectBiomes(double temperature, double humidity) const;
private:
  World &m_world;
  std::unordered_map<BiomeType, Biome> m_biomes;
  std::unordered_map<glm::ivec2, std::vector<std::pair<glm::vec3, BlockType>>, Utils::IVec2Hash> m_unloadedBlocks;

  bool CanTreeSpawn(Chunk &chunk, int x, int surfaceY, int z, int radius);
  void SpawnTree(Chunk &chunk, int x, int surfaceY, int z);
};

}

#endif // WORLD_GENERATION_H_
