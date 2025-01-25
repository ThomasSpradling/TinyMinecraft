#include "World/Biome.h"
#include "Utils/defs.h"
#include "Utils/utils.h"
#include "World/Block.h"
#include "Utils/mathgl.h"
#include <algorithm>

namespace World {

Biome::Biome(BiomeType type, float minHeight, float maxHeight, 
    float minTemp, float maxTemp, 
    float minHumidity, float maxHumidity) : type(type),
  minHeight(minHeight), maxHeight(maxHeight),
  minTemp(minTemp), maxTemp(maxTemp),
  minHumidity(minHumidity), maxHumidity(maxHumidity) {}

bool Biome::IsValid(float temperature, float humidity) const {
  return temperature >= minTemp && temperature <= maxTemp &&
         humidity >= minHumidity && humidity <= maxHumidity;
}

float Biome::GenerateHeight(int x, int z, PerlinNoise &heightMap) const {
  float height = Utils::OctaveNoise(x * 0.005, z * 0.005, heightMap, 4);
  // height = glm::pow(height * 1.2, 4.9);
  height = Utils::ScaleValue(0, 1, minHeight, maxHeight, height);
  height = std::clamp(height, 0.f, static_cast<float>(CHUNK_HEIGHT));
  
  return height;
}

Block Biome::GenerateBlock(int x, int y, int z, int height, float stoneNoise) const {
  const int seaLevel = 62;
  const float stoneThreshold = 0.75;
  
  switch(type) {
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
  return BlockType::AIR;
}

BiomeType Biome::GetType() const {
  return type;
}

float Biome::GetMinHeight() const {
  return minHeight;
}

float Biome::GetMaxHeight() const {
  return maxHeight;
}

float Biome::GetMinTemperature() const {
  return minTemp;
}

float Biome::GetMaxTemperature() const {
  return maxTemp;
}

float Biome::GetMinHumidity() const {
  return minHumidity;
}

float Biome::GetMaxHumidty() const {
  return maxHumidity;
}

std::string Biome::GetBiomeName(BiomeType type) {
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
