#include "World/Biome.h"
#include "Utils/defs.h"
#include "Utils/utils.h"
#include "World/Block.h"
#include "Utils/mathgl.h"
#include <algorithm>

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

auto Biome::GenerateHeight(int x, int z, PerlinNoise &heightMap) const -> double {
  double height = Utils::OctaveNoise(x * 0.005, z * 0.005, heightMap, 4);
  // height = glm::pow(height * 1.2, 4.9);
  height = Utils::ScaleValue(0.0, 1.0, m_minHeight, m_maxHeight, height);
  height = std::clamp(height, 0.0, static_cast<double>(CHUNK_HEIGHT));
  
  return height;
}

auto Biome::GenerateBlock(int x, int y, int z, int height, double stoneNoise) const -> Block {
  const int seaLevel = 62;
  const float stoneThreshold = 0.75;

// NOLINTBEGIN(bugprone-branch-clone)  

  switch(m_type) {
    case BiomeType::Desert:
      if (y >= height - 2 && y <= height) return BlockType::SAND;
      if (y < height - 2) return BlockType::STONE;
    case BiomeType::Grassland:
      if (stoneNoise > stoneThreshold && y <= height) return BlockType::STONE;

      if (height <= seaLevel && y == height) return BlockType::SAND;
      if (height < seaLevel && y > height && y <= seaLevel) return BlockType::WATER;

      if (y == height) return BlockType::GRASS;
      if (y >= height - 4 && y < height) return BlockType::DIRT;
      if (y < height - 2) return BlockType::STONE;

      break;
    case BiomeType::Savanna:
      if (stoneNoise > stoneThreshold && y <= height) return BlockType::STONE;

      if (height <= seaLevel && y == height) return BlockType::SAND;
      if (height < seaLevel && y > height && y <= seaLevel) return BlockType::WATER;

      if (y == height) return BlockType::GRASS;
      if (y >= height - 4 && y < height) return BlockType::DIRT;
      if (y < height - 2) return BlockType::STONE;

      break;
    case BiomeType::Swamp:
      if (height < seaLevel && y < height) return BlockType::WATER;
      if (height <= seaLevel && y == height) return BlockType::SAND;

      if (y == height) return BlockType::GRASS;
      if (y >= height - 2 && y < height) return BlockType::DIRT;
      if (y < height - 2) return BlockType::STONE;

      break;
    case BiomeType::Tundra:
      if (y >= height - 2 && y <= height) return BlockType::SNOW;
      if (y >= height - 5 && y < height - 2) return BlockType::DIRT;
      if (y < height - 2) return BlockType::STONE;
      break;

    case BiomeType::Taiga:
      if (y >= height - 2 && y <= height) return BlockType::LOG;
      if (y >= height - 5 && y < height - 2) return BlockType::DIRT;
      if (y < height - 2) return BlockType::STONE;
      break;
    case BiomeType::Shrubland:
      if (y >= height - 2 && y <= height) return BlockType::GRASS;
      if (y >= height - 5 && y < height - 2) return BlockType::DIRT;
      if (y < height - 2) return BlockType::STONE;
      break;
    case BiomeType::Forest:
      if (y >= height - 2 && y <= height) return BlockType::LOG;
      if (y >= height - 5 && y < height - 2) return BlockType::DIRT;
      if (y < height - 2) return BlockType::STONE;
      break;
    default:
  }
// NOLINTEND(bugprone-branch-clone)  

  return BlockType::AIR;
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
