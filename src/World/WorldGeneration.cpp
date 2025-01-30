#include "World/WorldGeneration.h"

#include "Utils/Profiler.h"
#include "Utils/defs.h"
#include "Utils/mathgl.h"
#include "Utils/noise.h"
#include "World/Biome.h"
#include "World/Block.h"
#include "World/Chunk.h"
#include "World/World.h"
#include <array>
#include <cstddef>
#include <string>
#include <unordered_set>
#include <utility>

namespace World {

WorldGeneration::WorldGeneration(World &world) : m_world(world) {
  // load splines

  m_biomes.emplace(BiomeType::Tundra, Biome(BiomeType::Tundra, 
                                          35.0f, 100.0f, 0.0, 0.1, 0.0, 1.0));
  m_biomes.emplace(BiomeType::Taiga, Biome(BiomeType::Taiga,
                                          35.0f, 100.0f, 0.1f, 0.4f, 0.40f, 1.0f));
  m_biomes.emplace(BiomeType::Swamp, Biome(BiomeType::Swamp,
                                          35.0f, 100.0f, 0.4f, 0.7f, 0.7f, 1.0f));
  m_biomes.emplace(BiomeType::Shrubland, Biome(BiomeType::Shrubland,
                                          35.0f, 100.0f, 0.4f, 0.75f, 0.40f, 0.7f));
  m_biomes.emplace(BiomeType::Savanna, Biome(BiomeType::Savanna,
                                          35.0f, 100.0f, 0.75, 1.0, 0.4, 0.7));
  m_biomes.emplace(BiomeType::Forest, Biome(BiomeType::Forest,
                                          35.0f, 100.0f, 0.7f, 1.0f, 0.7f, 1.0f));
  m_biomes.emplace(BiomeType::Grassland, Biome(BiomeType::Grassland,
                                          35.0f, 100.0f, 0.10f, 0.6f, 0.0f, 0.40f));
  m_biomes.emplace(BiomeType::Desert, Biome(BiomeType::Desert,
                                          35.0f, 100.0f, 0.6f, 1.0f, 0.0f, 0.40f));
}

void WorldGeneration::GenerateTerrainChunk(Chunk &chunk, PerlinNoise &blendMap, PerlinNoise &heightMap, PerlinNoise &stoneMap) {
  PROFILE_FUNCTION(Chunk)
  
  const glm::ivec2 &chunkPos = chunk.GetChunkPos();

  for (int z = 0; z < CHUNK_LENGTH; ++z) {
    for (int x = 0; x < CHUNK_WIDTH; ++x) {
      int nx = chunkPos.x * static_cast<int>(CHUNK_WIDTH) + x;
      int nz = chunkPos.y * static_cast<int>(CHUNK_LENGTH) + z;

      double temperature = m_world.GetTemperature(nx, nz);
      double humidity = m_world.GetHumidity(nx, nz);

      m_world.GetHumidity(nx, nz);

      auto [primaryBiome, secondaryBiome] = SelectBiomes(temperature, humidity);
      // if (primaryBiome == nullptr || secondaryBiome == nullptr) {
      //   continue;
      // }
    
      double primaryHeight = primaryBiome->GenerateHeight(nx, nz, heightMap);
      // float secondaryHeight = secondaryBiome->GenerateHeight(nx, nz, heightMap);

      // float blendFactor = Utils::OctaveNoise(nx * 0.5, nz * 0.5, blendMap);

      // float height = primaryHeight * (1 - blendFactor) + secondaryHeight * blendFactor;

      double stoneNoise = Utils::OctaveNoise(nx * 0.01, nz * 0.01, stoneMap, 2);
      for (int y = 0; y < CHUNK_HEIGHT; ++y) {
        chunk.SetBlockAt(x, y, z, primaryBiome->GenerateBlock(nx, y, nz, static_cast<int>(primaryHeight), stoneNoise));

        // if (x == 5 && y > 60 && z < 80 && chunkPos == glm::ivec2(0,0)) {
        //   chunk.SetBlockAt(x, y, z, BlockType::GLASS);
        // }
      }

    }
  }
}

void WorldGeneration::GenerateFeatures(Chunk &chunk, PerlinNoise &treeMap, PerlinNoise &grassMap) {
  // for grass blocks, use perlin noise to define if tree should go there

  constexpr float TREE_GENERATION_THRESHOLD = 0.73;
  constexpr int TREE_RADIUS = 2;

  std::unordered_set<glm::ivec2, Utils::IVec2Hash> reservedPositions;

  for (int z = 0; z < CHUNK_LENGTH; ++z) {
    for (int x = 0; x < CHUNK_WIDTH; ++x) {

      int globalX = CHUNK_WIDTH * chunk.GetChunkPos().x + x;
      int globalZ = CHUNK_WIDTH * chunk.GetChunkPos().y + z;

       if (!reservedPositions.contains(glm::ivec2(x, z)) && Utils::OctaveNoise(globalX, globalZ, grassMap) > 0.65) {
        int surfaceHeight = chunk.GetSurfaceHeight(x, z);

        if (chunk.GetBlockAt(x, surfaceHeight, z).GetType() == BlockType::GRASS) {
          chunk.SetBlockAt(x, surfaceHeight + 1, z, BlockType::TALL_GRASS);
          reservedPositions.insert(glm::ivec2(x, z));
        }

      }

      if (reservedPositions.contains(glm::ivec2(x, z))) {
        continue;
      }

      if (Utils::OctaveNoise(globalX, globalZ, treeMap) < TREE_GENERATION_THRESHOLD) {
        continue;
      }

      int surfaceY = chunk.GetSurfaceHeight(x, z);
      if (!CanTreeSpawn(chunk, x, surfaceY, z, TREE_RADIUS)) {
        continue;
      }

      SpawnTree(chunk, x, surfaceY, z);
      for (int dx = -TREE_RADIUS; dx <= TREE_RADIUS; ++dx) {
        for (int dz = -TREE_RADIUS; dz <= TREE_RADIUS; ++dz) {
          int localX = dx + x;
          int localZ = dz + z;

          if (localX < 0 || localZ < 0 || localX >= CHUNK_WIDTH || localZ >= CHUNK_LENGTH) {
            continue;
          }

          reservedPositions.insert(glm::ivec2(dx + x, dz + z));
        }
      }
    }
  }
}

void WorldGeneration::LoadUnloadedBlocks(Chunk &chunk) {
  glm::ivec2 chunkPos = chunk.GetChunkPos();

  if (!m_unloadedBlocks.contains(chunkPos)) {
    return;
  }

  auto a = m_unloadedBlocks.at(chunkPos);

  for (auto &[pos, block] : m_unloadedBlocks.at(chunkPos)) {
    glm::vec3 globalPos = glm::vec3(CHUNK_WIDTH * chunkPos.x + pos.x, pos.y, CHUNK_WIDTH * chunkPos.y + pos.y);
    chunk.SetBlockAt(pos, block);
  }

  m_unloadedBlocks.erase(chunkPos);
}

auto WorldGeneration::SelectBiomes(double temperature, double humidity) const -> std::pair<const Biome*, const Biome*> {
  const Biome *primaryBiome = nullptr;
  const Biome *secondaryBiome = nullptr;
  // primaryBiome = &biomes.at(BiomeType::Grassland);
  // return {primaryBiome, secondaryBiome};

  for (const auto &[type, biome] : m_biomes) {
    if (biome.IsValid(temperature, humidity)) {
      if (!primaryBiome || std::abs(temperature - biome.GetMinTemperature()) < std::abs(temperature - primaryBiome->GetMinTemperature())) {
        secondaryBiome = primaryBiome;
        primaryBiome = &biome;
      } else if (!secondaryBiome) {
        secondaryBiome = &biome;
      }
    }

  }
  
  if (!primaryBiome) primaryBiome = &m_biomes.at(BiomeType::Desert);
  if (!secondaryBiome) secondaryBiome = primaryBiome;
  return {primaryBiome, secondaryBiome};
}

void WorldGeneration::SpawnTree(Chunk &chunk, int x, int surfaceY, int z) {

  int structure[7][5][5] = {
    {
      {0,      0,      0,      0,    0},
      {0,      0,      0,      0,    0},
      {0,      0,      LOG,    0,    0},
      {0,      0,      0,      0,    0},
      {0,      0,      0,      0,    0},
    },
    {
      {0,      0,      0,      0,    0},
      {0,      0,      0,      0,    0},
      {0,      0,      LOG,    0,    0},
      {0,      0,      0,      0,    0},
      {0,      0,      0,      0,    0},
    },
    {
      {0,      0,      0,      0,    0},
      {0,      0,      0,      0,    0},
      {0,      0,      LOG,    0,    0},
      {0,      0,      0,      0,    0},
      {0,      0,      0,      0,    0},
    },
    {
      {0,      LEAVES, LEAVES, LEAVES,       0},
      {LEAVES, LEAVES, LEAVES, LEAVES,    LEAVES},
      {LEAVES, LEAVES, LOG,    LEAVES,      LEAVES},
      {LEAVES, LEAVES, LEAVES, LEAVES,    LEAVES},
      {0,      LEAVES, LEAVES, LEAVES,       0},
    },
    {
      {0,       LEAVES,       LEAVES,       LEAVES,       0},
      {LEAVES,      LEAVES,     LEAVES,     LEAVES,    LEAVES},
      {LEAVES,      LEAVES,     LOG,      LEAVES,      LEAVES},
      {LEAVES,      LEAVES,     LEAVES,     LEAVES,    LEAVES},
      {0,       LEAVES,       LEAVES,       LEAVES,       0},
    },
    {
      {0,       0,      0,      0,    0},
      {0,       LEAVES,       LEAVES,       LEAVES,       0},
      {0,       LEAVES,       LOG,      LEAVES,     0},
      {0,       LEAVES,       LEAVES,       LEAVES,       0},
      {0,       0,      0,      0,    0},
    },
    {
      {0,       0,      0,      0,    0},
      {0,       0,      LEAVES,     0,      0},
      {0,       LEAVES,       LEAVES,       LEAVES,       0},
      {0,       0,      LEAVES,     0,      0},
      {0,       0,      0,      0,    0},
    },
  };

  for (int dy = 0; dy < 7; ++dy) {
    for (int dx = -2; dx <= 2; ++dx) {
      for (int dz = -2; dz <= 2; ++dz) {
        int localX = x + dx;
        int localZ = z + dz;
        int localY = surfaceY + 1 + dy;

        auto block = static_cast<BlockType>(structure[dy][dx + 2][dz + 2]);

        if (block == 0) {
          continue;
        }

        if (localY < 0 || localY >= CHUNK_HEIGHT) {
          continue;
        }

        // place this in list of blocks to be loaded again later

        if (localX < 0 || localZ < 0 || localX >= CHUNK_WIDTH || localZ >= CHUNK_LENGTH) {
          int globalX = chunk.GetChunkPos().x * CHUNK_WIDTH + localX;
          int globalY = localY;
          int globalZ = chunk.GetChunkPos().y * CHUNK_LENGTH + localZ;


          glm::vec3 globalPos { globalX, globalY, globalZ };
          glm::ivec2 neighborChunkPos = m_world.GetChunkPosFromCoords(globalPos);
          glm::vec3 neighborLocalPos = m_world.GetLocalBlockCoords(globalPos);

          m_unloadedBlocks[neighborChunkPos].emplace_back(neighborLocalPos, block);
          continue;
        }

        chunk.SetBlockAt(localX, localY, localZ, block);
      }
    }
  }
}

auto WorldGeneration::CanTreeSpawn(Chunk &chunk, int x, int surfaceY, int z, int radius) -> bool {
  if (chunk.GetBlockAt(x, surfaceY, z).GetType() != BlockType::GRASS) {
    return false;
  }

  for (int dx = -radius; dx <= radius; ++dx) {
    for (int dz = -radius; dz <= radius; ++dz) {
      int localX = x + dx;
      int localZ = z + dz;
      if (localX < 0 || localZ < 0 || localX >= CHUNK_WIDTH || localZ >= CHUNK_LENGTH) {
        continue;
      }

      if (chunk.GetBlockAt(x, surfaceY + 1, z).GetType() != BlockType::AIR) {
        return false;
      }
    }
  }

  return true;
}

}