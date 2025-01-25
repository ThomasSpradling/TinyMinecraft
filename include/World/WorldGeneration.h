#ifndef WORLD_GENERATION_H_
#define WORLD_GENERATION_H_

#include "Utils/noise.h"
#include "Utils/mathgl.h"
#include "World/Biome.h"

namespace World {

class World;

class WorldGeneration {
public:
  WorldGeneration(World &world);
  void GenerateTerrainChunk(glm::ivec2 &chunkPos, PerlinNoise &blendMap, PerlinNoise &heightMap, PerlinNoise &stoneMap);
  std::pair <const Biome*, const Biome*> SelectBiomes(float temperature, float humidity);
private:
  World &world;
  std::unordered_map<BiomeType, Biome> biomes;
};

}

#endif // WORLD_GENERATION_H_
