#include "World/WorldGeneration.h"

#include "FastNoise/FastNoise_Config.h"
#include "FastNoise/Generators/Fractal.h"
#include "FastNoise/Generators/Perlin.h"
#include "FastNoise/Generators/Simplex.h"
#include "Utils/Logger.h"
#include "Utils/Profiler.h"
#include "Utils/defs.h"
#include "Utils/mathgl.h"
#include "Utils/utils.h"
#include "World/Biome.h"
#include "World/Block.h"
#include "World/Chunk.h"
#include "World/World.h"
#include "Math/splines.h"
#include <array>
#include <cstddef>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_set>
#include <utility>

namespace TinyMinecraft {

  namespace World {

    // FastNoise::SmartNode<FastNoise::Perlin> WorldGeneration::s_simplex = FastNoise::New<FastNoise::Perlin>();

    WorldGeneration::WorldGeneration(World &world) : m_world(world) {
      // auto m_baseTerrain = FastNoise::New<FastNoise::FractalFBm>();
      // m_baseTerrain->SetSource(terrain);
      // m_baseTerrain->SetOctaveCount(1);

      auto terrain = FastNoise::New<FastNoise::Perlin>();
      m_baseTerrain = FastNoise::New<FastNoise::FractalFBm>();
      m_baseTerrain->SetSource(terrain);

      m_ridges = FastNoise::New<FastNoise::Perlin>();

      m_caves = FastNoise::New<FastNoise::OpenSimplex2S>();
      // m_caves = FastNoise::New<FastNoise::DomainScale>();
      // m_caves->SetScale(0.8f);
      // m_caves->SetSource(m_caves);
      m_featureNoise = FastNoise::New<FastNoise::Simplex>();

      // load splines
      std::ifstream file("../data/world_gen.json");
      if (!file.is_open()) {
        Utils::g_logger.Error("Conig: file cannot open");
        exit(1);
      }
    
      nlohmann::json data;
      file >> data;

      auto &funcs = data["world_generation"]["noise_functions"];
      auto &continentalnessSplines = funcs["continentalness"]["splines"];
      auto &continentalnessFunction = funcs["continentalness"]["method"];

      auto &erosionSplines = funcs["erosion"]["splines"];
      auto &erosionFunction = funcs["erosion"]["method"];

      auto &ridgesSplines = funcs["ridges"]["splines"];
      auto &ridgesFunction = funcs["ridges"]["method"];

      static std::vector<glm::vec2> continentalnessSplinePoints;
      static std::vector<glm::vec2> erosionSplinePoints;
      static std::vector<glm::vec2> ridgesSplinePoints;

      continentalnessSplinePoints.reserve(continentalnessSplines.size());
      erosionSplinePoints.reserve(erosionSplines.size());
      ridgesSplinePoints.reserve(ridgesSplines.size());

      for (auto &xy : continentalnessSplines)
        continentalnessSplinePoints.emplace_back(xy[0], xy[1]);

      for (auto &xy : erosionSplines)
        erosionSplinePoints.emplace_back(xy[0], xy[1]);

      for (auto &xy : erosionSplines)
        ridgesSplinePoints.emplace_back(xy[0], xy[1]);

      static std::vector<glm::vec4> continentalnessCoeffs = Math::ComputeMonotonicCubicSplines(continentalnessSplinePoints);
      static std::vector<glm::vec4> erosionCoeffs = Math::ComputeMonotonicCubicSplines(erosionSplinePoints);
      static std::vector<glm::vec4> ridgesCoeffs = Math::ComputeCubicSplines(erosionSplinePoints);

      ContinentalnessPart = [&](float continentalness) {
        return Math::EvaluateCubicSpline(continentalnessSplinePoints, continentalnessCoeffs, continentalness);
      };

      ErosionPart = [&](float erosion) {
        return Math::EvaluateCubicSpline(erosionSplinePoints, erosionCoeffs, erosion);
      };

      RidgesPart = [&](float erosion) {
        return Math::EvaluateCubicSpline(erosionSplinePoints, erosionCoeffs, erosion);
      };

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

    void WorldGeneration::GenerateTerrainChunk(Chunk *chunk) {
      PROFILE_FUNCTION(Chunk)

      chunk->ReserveBlocks();
      
      const glm::ivec2 &chunkPos = chunk->GetChunkPos();

      std::vector<float> continentalnessMap(16 * 16);
      std::vector<float> peaksMap(16 * 16);
      std::vector<float> erosionMap(16 * 16);

      m_baseTerrain->GenUniformGrid2D(continentalnessMap.data(), chunkPos.x * 16.0f, chunkPos.y * 16.0f, 16.0f, 16.0f, 0.1f / 16.0f, 1334);
      m_baseTerrain->GenUniformGrid2D(peaksMap.data(), chunkPos.x * 16.0f, chunkPos.y * 16.0f, 16.0f, 16.0f, 0.04f / 16.0f, 1335);
      m_baseTerrain->GenUniformGrid2D(erosionMap.data(), chunkPos.x * 16.0f, chunkPos.y * 16.0f, 16.0f, 16.0f, 0.1f / 16.0f, 1336);

      std::vector<float> terrain(16 * 16 * 256);
      std::vector<float> spaghettiCaves(16 * 16 * 256);
      std::vector<float> cheeseCaves(16 * 16 * 256);

      m_baseTerrain->GenUniformGrid3D(terrain.data(), 0, chunkPos.x * 16, chunkPos.y * 16, 256, 16, 16, 0.2f/16.0f, 1337);
      m_caves->GenUniformGrid3D(spaghettiCaves.data(), 0, chunkPos.x * 16, chunkPos.y * 16, 256, 16, 16, 0.8f/16.0f, 1339);
      m_caves->GenUniformGrid3D(cheeseCaves.data(), 0, chunkPos.x * 16, chunkPos.y * 16, 256, 16, 16, 0.8f/16.0f, 1340);

      int index2D = 0;
      int index = 0;
      double caveThickness = 0.1;
      double cavesSize = 0.6f;

      for (int z = 0; z < CHUNK_LENGTH; ++z) {
        for (int x = 0; x < CHUNK_WIDTH; ++x) {
          int nx = chunkPos.x * static_cast<int>(CHUNK_WIDTH) + x;
          int nz = chunkPos.y * static_cast<int>(CHUNK_LENGTH) + z;

          double continentalness = continentalnessMap[index2D];
          continentalness = Utils::ScaleValue(-1.0, 1.0, 0.0, 1.0, continentalness);
          double continentalnessHeight = ContinentalnessPart(continentalness);
          continentalnessHeight = Utils::ScaleValue(0.25, 1.0, 0.0, 1.0, continentalnessHeight);

          double erosion = erosionMap[index2D];
          erosion = Utils::ScaleValue(-1.0, 1.0, 0.0, 1.0, erosion);
          double erosionHeight = ErosionPart(erosion);
          erosionHeight = Utils::ScaleValue(0.25, 1.0, 0.0, 1.0, erosionHeight);

          double peaks = peaksMap[index2D];
          peaks = 1 - std::fabs(3 * std::fabs(peaks) - 2);
          peaks = Utils::ScaleValue(-1.0, 1.0, 0.0, 1.0, peaks);
          double peaksHeight = RidgesPart(peaks);
          peaksHeight = Utils::ScaleValue(0.0, 1.0, -1.0, 1.0, peaksHeight);

          int baseHeight = 64.f + (erosionHeight + 0.75f * continentalnessHeight + 0.5f * peaksHeight) * 50.0f;

          for (int y = 0; y < CHUNK_HEIGHT; ++y) {
            caveThickness = Utils::ScaleValue(0.0f, 62.0f, 0.4f, 0.0f, static_cast<float>(y));
            cavesSize = Utils::ScaleValue(0.0f, 62.0f, 0.6f, 0.3f, static_cast<float>(y));

            double terrainNoise = terrain[index];
            // double ridgeNoise = std::fabs(ridges[index]);

            double density = terrainNoise - (y - baseHeight) / 32.0f;
            double spaghettiNoise = spaghettiCaves[index];
            double cheeseNoise = cheeseCaves[index];

            if (cheeseNoise > 1 - cavesSize && y < 62.0f || spaghettiNoise > -caveThickness && spaghettiNoise < caveThickness && y < 62.0f) {
              density = 0;
            }
          
            if (density > 0.0f) {
              chunk->SetBlockAt(glm::vec3(x, y, z), BlockType::GRASS);
            } else {
              if (y <= 62 && y >= baseHeight - 10) {
                chunk->SetBlockAt(glm::vec3(x, y, z), BlockType::WATER);
              }
            }

            index++;
          }
          index2D++;
        }
      }
    }

    void WorldGeneration::GenerateFeatures(Chunk *chunk) {
      PROFILE_FUNCTION(Chunk)
      
      constexpr float TREE_GENERATION_THRESHOLD = 0.95f;
      constexpr float GRASS_GENERATION_THRESHOLD = 0.80f;
      constexpr int TREE_RADIUS = 2;
      constexpr float TREE_NOISE_SCALE = 10.0f;
      constexpr float GRASS_NOISE_SCALE = 100.0f;

      std::unordered_set<glm::ivec2, Utils::IVec2Hash> reservedPositions;

      for (int z = 0; z < CHUNK_LENGTH; ++z) {
        for (int x = 0; x < CHUNK_WIDTH; ++x) {
          glm::ivec2 position(x, z);

          if (reservedPositions.find(position) != reservedPositions.end()) {
            continue;
          }

          int globalX = CHUNK_WIDTH * chunk->GetChunkPos().x + x;
          int globalZ = CHUNK_WIDTH * chunk->GetChunkPos().y + z;

          double grassNoise = m_featureNoise->GenSingle2D(globalX, globalZ, 100);
          grassNoise = Utils::ScaleValue(-1.0, 1.0, 0.0, 1.0, grassNoise);

          // double grassNoise = Math::NoiseManager::GetImprovedSimplexNoise(Math::Noise::Grass, glm::vec2(globalX * GRASS_NOISE_SCALE, globalZ * GRASS_NOISE_SCALE));
          if (grassNoise > GRASS_GENERATION_THRESHOLD) {
            int surfaceHeight = chunk->GetSurfaceHeight(x, z);

            if (chunk->GetBlockAt(x, surfaceHeight, z).GetType() == BlockType::GRASS) {
              chunk->SetBlockAt(x, surfaceHeight + 1, z, BlockType::TALL_GRASS);
              reservedPositions.insert(position);
            }
          }

          if (reservedPositions.find(position) != reservedPositions.end()) {
            continue;
          }

          double treeNoise = m_featureNoise->GenSingle2D(globalX / 16.0f, globalZ / 16.0f, 101);
          treeNoise = Utils::ScaleValue(-1.0, 1.0, 0.0, 1.0, treeNoise);
          // double treeNoise = Math::NoiseManager::GetImprovedSimplexNoise(Math::Noise::Tree, glm::vec2(globalX * TREE_NOISE_SCALE, globalZ * TREE_NOISE_SCALE));
          if (treeNoise >= TREE_GENERATION_THRESHOLD) {
            int surfaceY = chunk->GetSurfaceHeight(x, z);

            if (CanTreeSpawn(chunk, x, surfaceY, z, TREE_RADIUS)) {
              SpawnTree(chunk, x, surfaceY, z);
              for (int dx = -TREE_RADIUS; dx <= TREE_RADIUS; ++dx) {
                for (int dz = -TREE_RADIUS; dz <= TREE_RADIUS; ++dz) {
                  int localX = x + dx;
                  int localZ = z + dz;

                  if (localX < 0 || localZ < 0 || localX >= CHUNK_WIDTH || localZ >= CHUNK_LENGTH) {
                    continue;
                  }
                  reservedPositions.insert(glm::ivec2(localX, localZ));
                }
              }
            }
          }
        }
      }
    }

    void WorldGeneration::LoadUnloadedBlocks(Chunk *chunk) {
      glm::ivec2 chunkPos = chunk->GetChunkPos();

      if (!m_unloadedBlocks.contains(chunkPos)) {
        return;
      }

      auto a = m_unloadedBlocks.at(chunkPos);

      for (auto &[pos, block] : m_unloadedBlocks.at(chunkPos)) {
        glm::vec3 globalPos = glm::vec3(CHUNK_WIDTH * chunkPos.x + pos.x, pos.y, CHUNK_WIDTH * chunkPos.y + pos.y);
        chunk->SetBlockAt(pos, block);
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

    void WorldGeneration::SpawnTree(Chunk *chunk, int x, int surfaceY, int z) {

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
              int globalX = chunk->GetChunkPos().x * CHUNK_WIDTH + localX;
              int globalY = localY;
              int globalZ = chunk->GetChunkPos().y * CHUNK_LENGTH + localZ;


              glm::vec3 globalPos { globalX, globalY, globalZ };
              glm::ivec2 neighborChunkPos = m_world.GetChunkPosFromCoords(globalPos);
              glm::vec3 neighborLocalPos = m_world.GetLocalBlockCoords(globalPos);

              m_unloadedBlocks[neighborChunkPos].emplace_back(neighborLocalPos, block);
              continue;
            }

            chunk->SetBlockAt(localX, localY, localZ, block);
          }
        }
      }
    }

    auto WorldGeneration::CanTreeSpawn(Chunk *chunk, int x, int surfaceY, int z, int radius) -> bool {
      if (chunk->GetBlockAt(x, surfaceY, z).GetType() != BlockType::GRASS) {
        return false;
      }

      for (int dx = -radius; dx <= radius; ++dx) {
        for (int dz = -radius; dz <= radius; ++dz) {
          int localX = x + dx;
          int localZ = z + dz;
          if (localX < 0 || localZ < 0 || localX >= CHUNK_WIDTH || localZ >= CHUNK_LENGTH) {
            continue;
          }

          if (chunk->GetBlockAt(x, surfaceY + 1, z).GetType() != BlockType::AIR) {
            return false;
          }
        }
      }

      return true;
    }

  }

}
