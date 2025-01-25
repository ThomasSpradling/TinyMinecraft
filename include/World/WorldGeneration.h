#ifndef WORLD_GENERATION_H_
#define WORLD_GENERATION_H_

#include "Utils/noise.h"
#include "Utils/mathgl.h"
#include "World/Biome.h"
#include "World/Chunk.h"

namespace World {

class World;

class WorldGeneration {
public:
  WorldGeneration(World &world);
  void GenerateTerrainChunk(Chunk &chunk, PerlinNoise &blendMap, PerlinNoise &heightMap, PerlinNoise &stoneMap);
  std::pair <const Biome*, const Biome*> SelectBiomes(float temperature, float humidity) const;
private:
  World &world;
  std::unordered_map<BiomeType, Biome> biomes;
};

}

#endif // WORLD_GENERATION_H_
