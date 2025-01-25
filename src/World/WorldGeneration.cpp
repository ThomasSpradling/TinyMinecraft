#include "World/WorldGeneration.h"

#include "Utils/Profiler.h"
#include "Utils/defs.h"
#include "Utils/noise.h"
#include "Utils/utils.h"
#include "World/Biome.h"
#include "World/Chunk.h"
#include "World/World.h"
#include <algorithm>
#include <cstddef>
#include <iostream>

namespace World {

WorldGeneration::WorldGeneration(World &world) : world(world) {
  // biomes.emplace(BiomeType::Desert, Biome(BiomeType::Desert, 
  //                                         20.0f, 40.0f, 0.7f, 1.0f, 0.0f, 0.4f));
  // biomes.emplace(BiomeType::Plains, Biome(BiomeType::Plains,
  //                                         10.0f, 25.0f, 0.3f, 0.7f, 0.4f, 0.8f));
  // biomes.emplace(BiomeType::Swamp, Biome(BiomeType::Swamp,
  //                                         5.0f, 15.0f, 0.2f, 0.6f, 0.6f, 1.0f));
  // biomes.emplace(BiomeType::Tundra, Biome(BiomeType::Tundra,
  //                                         0.0f, 10.0f, 0.0f, 0.3f, 0.0f, 0.5f));

  biomes.emplace(BiomeType::Tundra, Biome(BiomeType::Tundra, 
                                          60.0f, 150.0f, 0.0, 0.1, 0.0, 1.0));
  biomes.emplace(BiomeType::Taiga, Biome(BiomeType::Taiga,
                                          60.0f, 200.0f, 0.1f, 0.4f, 0.40f, 1.0f));
  biomes.emplace(BiomeType::Swamp, Biome(BiomeType::Swamp,
                                          60.0f, 200.0f, 0.4f, 0.7f, 0.7f, 1.0f));
  biomes.emplace(BiomeType::Shrubland, Biome(BiomeType::Shrubland,
                                          60.0f, 200.0f, 0.4f, 0.75f, 0.40f, 0.7f));
  biomes.emplace(BiomeType::Savanna, Biome(BiomeType::Savanna,
                                          60.0f, 200.0f, 0.75, 1.0, 0.4, 0.7));
  biomes.emplace(BiomeType::Forest, Biome(BiomeType::Forest,
                                          60.0f, 200.0f, 0.7f, 1.0f, 0.7f, 1.0f));
  biomes.emplace(BiomeType::Grassland, Biome(BiomeType::Grassland,
                                          35.0f, 100.0f, 0.10f, 0.6f, 0.0f, 0.40f));
  biomes.emplace(BiomeType::Desert, Biome(BiomeType::Desert,
                                          60.0f, 200.0f, 0.6f, 1.0f, 0.0f, 0.40f));
}

void WorldGeneration::GenerateTerrainChunk(Chunk &chunk, PerlinNoise &blendMap, PerlinNoise &heightMap, PerlinNoise &stoneMap) {
  PROFILE_FUNCTION(Chunk)
  
  const glm::ivec2 &chunkPos = chunk.GetChunkPos();

  for (int z = 0; z < CHUNK_LENGTH; ++z) {
    for (int x = 0; x < CHUNK_WIDTH; ++x) {
      int nx = chunkPos.x * static_cast<int>(CHUNK_WIDTH) + x;
      int nz = chunkPos.y * static_cast<int>(CHUNK_LENGTH) + z;

      float temperature = world.GetTemperature(nx, nz);
      float humidity = world.GetHumidity(nx, nz);

      world.GetHumidity(nx, nz);

      auto [primaryBiome, secondaryBiome] = SelectBiomes(temperature, humidity);
      // if (primaryBiome == nullptr || secondaryBiome == nullptr) {
      //   continue;
      // }
    
      float primaryHeight = primaryBiome->GenerateHeight(nx, nz, heightMap);
      // float secondaryHeight = secondaryBiome->GenerateHeight(nx, nz, heightMap);

      // float blendFactor = Utils::OctaveNoise(nx * 0.5, nz * 0.5, blendMap);

      // float height = primaryHeight * (1 - blendFactor) + secondaryHeight * blendFactor;

      float stoneNoise = Utils::OctaveNoise(nx * 0.01, nz * 0.01, stoneMap, 2);
      for (int y = 0; y < CHUNK_HEIGHT; ++y) {
        chunk.SetBlockAt(x, y, z, primaryBiome->GenerateBlock(nx, y, nz, primaryHeight, stoneNoise));
      }
    }
  }
}

std::pair<const Biome*, const Biome*> WorldGeneration::SelectBiomes(float temperature, float humidity) const {
  const Biome *primaryBiome = nullptr;
  const Biome *secondaryBiome = nullptr;
  primaryBiome = &biomes.at(BiomeType::Grassland);
  return {primaryBiome, secondaryBiome};

  for (const auto &[type, biome] : biomes) {
    if (biome.IsValid(temperature, humidity)) {
      if (!primaryBiome || std::abs(temperature - biome.GetMinTemperature()) < std::abs(temperature - primaryBiome->GetMinTemperature())) {
        secondaryBiome = primaryBiome;
        primaryBiome = &biome;
      } else if (!secondaryBiome) {
        secondaryBiome = &biome;
      }
    }

  }
  
  if (!primaryBiome) primaryBiome = &biomes.at(BiomeType::Desert);
  if (!secondaryBiome) secondaryBiome = primaryBiome;
  return {primaryBiome, secondaryBiome};
}

}