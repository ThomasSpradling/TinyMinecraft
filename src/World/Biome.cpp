#include "World/Biome.h"

namespace TinyMinecraft {

  namespace World {

    Biome::Biome(BiomeType type, float minHeight, float maxHeight, 
        float minTemp, float maxTemp, 
        float minHumidity, float maxHumidity)
      : m_type(type)
      , m_minHeight(minHeight)
      , m_maxHeight(maxHeight)
      , m_minTemp(minTemp)
      , m_maxTemp(maxTemp)
      , m_minHumidity(minHumidity)
      , m_maxHumidity(maxHumidity)
    {}

    auto Biome::IsValid(double temperature, double humidity) const -> bool {
      return temperature >= m_minTemp && temperature <= m_maxTemp &&
            humidity >= m_minHumidity && humidity <= m_maxHumidity;
    }

    auto Biome::GenerateBlock(int x, int y, int z, int height, double stoneNoise) const -> BlockType {
      const int seaLevel = 62;
      const float stoneThreshold = 0.75;
      if (height < seaLevel && y > height && y <= seaLevel) return BlockType::Water;

      switch(m_type) {
        
        case BiomeType::Desert:
          if (y >= height - 2 && y <= height) return BlockType::Sand;
          if (y < height - 2) return BlockType::Stone;
        case BiomeType::Grassland:
          if (stoneNoise > stoneThreshold && y <= height) return BlockType::Stone;

          if (height <= seaLevel && y == height) return BlockType::Sand;

          if (y == height) return BlockType::Grass;
          if (y >= height - 4 && y < height) return BlockType::Dirt;
          if (y < height - 2) return BlockType::Stone;

          break;
        case BiomeType::Savanna:
          if (stoneNoise > stoneThreshold && y <= height) return BlockType::Stone;

          if (height <= seaLevel && y == height) return BlockType::Sand;

          if (y == height) return BlockType::Grass;
          if (y >= height - 4 && y < height) return BlockType::Dirt;
          if (y < height - 2) return BlockType::Stone;

          break;
        case BiomeType::Swamp:
          // if (height < seaLevel && y < height) return BlockType::WATER;
          if (height <= seaLevel && y == height) return BlockType::Sand;

          if (y == height) return BlockType::Grass;
          if (y >= height - 2 && y < height) return BlockType::Dirt;
          if (y < height - 2) return BlockType::Stone;

          break;
        case BiomeType::Tundra:
          if (y >= height - 2 && y <= height) return BlockType::Snow;
          if (y >= height - 5 && y < height - 2) return BlockType::Dirt;
          if (y < height - 2) return BlockType::Stone;
          break;

        case BiomeType::Taiga:
          if (y >= height - 2 && y <= height) return BlockType::Log;
          if (y >= height - 5 && y < height - 2) return BlockType::Dirt;
          if (y < height - 2) return BlockType::Stone;
          break;
        case BiomeType::Shrubland:
          if (y >= height - 2 && y <= height) return BlockType::Grass;
          if (y >= height - 5 && y < height - 2) return BlockType::Dirt;
          if (y < height - 2) return BlockType::Stone;
          break;
        case BiomeType::Forest:
          if (y >= height - 2 && y <= height) return BlockType::Log;
          if (y >= height - 5 && y < height - 2) return BlockType::Dirt;
          if (y < height - 2) return BlockType::Stone;
          break;
        default:
          break;
      }

      return BlockType::Air;
    }

    auto Biome::GetType() const -> BiomeType {
      return m_type;
    }

    auto Biome::GetMinHeight() const -> double {
      return m_minHeight;
    }

    auto Biome::GetMaxHeight() const -> double {
      return m_maxHeight;
    }

    auto Biome::GetMinTemperature() const -> double {
      return m_minTemp;
    }

    auto Biome::GetMaxTemperature() const -> double {
      return m_maxTemp;
    }

    auto Biome::GetMinHumidity() const -> double {
      return m_minHumidity;
    }

    auto Biome::GetMaxHumidty() const -> double {
      return m_maxHumidity;
    }

    auto Biome::GetBiomeName(BiomeType type) -> std::string {
      switch (type) {
        case BiomeType::Desert:
          return "Desert";
        case BiomeType::Forest:
          return "Forest";
        case BiomeType::Grassland:
          return "Grassland";
        case BiomeType::Taiga:
          return "Taiga";
        case BiomeType::Tundra:
          return "Tundra";
        case BiomeType::Savanna:
          return "Savanna";
        case BiomeType::Shrubland:
          return "Shrubland";
        case BiomeType::Swamp:
          return "Swamp";
      }
    }

  }

}
