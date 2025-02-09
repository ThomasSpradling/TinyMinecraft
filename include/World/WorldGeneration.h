#ifndef WORLD_GENERATION_H_
#define WORLD_GENERATION_H_

#include <FastNoise/FastNoise.h>
#include "FastNoise/Generators/Fractal.h"
#include "FastNoise/Generators/Perlin.h"
#include "FastNoise/Generators/Simplex.h"
#include "FastNoise/SmartNode.h"
#include "Utils/Logger.h"
#include "Utils/mathgl.h"
#include "World/Biome.h"
#include "World/Block.h"
#include "World/Chunk.h"
#include <functional>

namespace TinyMinecraft {

  namespace World {

    class World;

    enum class SplineMethod {
      MonotonicNatural,
      Natural
    };

    class WorldGeneration {
    public:
      WorldGeneration(World &world);
      void GenerateTerrainChunk(Chunk *chunk);
      void GenerateFeatures(Chunk *chunk);

      void LoadUnloadedBlocks(Chunk *chunk);

      auto SelectBiomes(double temperature, double humidity) const -> std::pair<const Biome*, const Biome*>;
    private:
      World &m_world;
      std::unordered_map<BiomeType, Biome> m_biomes;
      std::unordered_map<glm::ivec2, std::vector<std::pair<glm::vec3, BlockType>>, Utils::IVec2Hash> m_unloadedBlocks;

      auto CanTreeSpawn(Chunk *chunk, int x, int surfaceY, int z, int radius) -> bool;
      void SpawnTree(Chunk *chunk, int x, int surfaceY, int z);
      [[nodiscard]] auto StringToSplineMethod(const std::string &method) -> SplineMethod {
        if (method == "monotonic_natural")
          return SplineMethod::MonotonicNatural;
        if (method == "natural")
          return SplineMethod::Natural;

        Utils::Logger::Error("Splines: invalid spline generation method \"{}\".", method);
        exit(1);
      }

      std::function<float(float)> ContinentalnessPart;
      std::function<float(float)> ErosionPart;
      std::function<float(float)> RidgesPart;

      FastNoise::SmartNode<FastNoise::Perlin> m_noise;
      FastNoise::SmartNode<FastNoise::FractalFBm> m_baseTerrain;
      FastNoise::SmartNode<FastNoise::Perlin> m_ridges;
      FastNoise::SmartNode<FastNoise::OpenSimplex2S> m_caves;
      FastNoise::SmartNode<FastNoise::Simplex> m_featureNoise;
    };

  }

}

#endif // WORLD_GENERATION_H_
