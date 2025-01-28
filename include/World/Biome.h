#ifndef BIOME_H_
#define BIOME_H_

#include "Utils/noise.h"
#include "World/Block.h"
namespace World {

enum class BiomeType : uint8_t {
  Tundra,
  Taiga,
  Grassland,
  Swamp,
  Shrubland,
  Forest,
  Savanna,
  Desert
};

class Biome {
public:
  Biome(BiomeType type, float minHeight, float maxHeight, 
    float minTemp, float maxTemp, 
    float minHumidity, float maxHumidity);

  [[nodiscard]] auto IsValid(double temperature, double humidity) const -> bool;
  auto GenerateHeight(int x, int z, PerlinNoise &heightMap) const -> double;
  [[nodiscard]] auto GenerateBlock(int x, int y, int z, int height, double stoneMap) const -> Block;

  [[nodiscard]] auto GetType() const -> BiomeType;

  [[nodiscard]] auto GetMinHeight() const -> double;
  [[nodiscard]] auto GetMaxHeight() const -> double;

  [[nodiscard]] auto GetMinTemperature() const -> double;
  [[nodiscard]] auto GetMaxTemperature() const -> double;

  [[nodiscard]] auto GetMinHumidity() const -> double;
  [[nodiscard]] auto GetMaxHumidty() const -> double;

  static auto GetBiomeName(BiomeType type) -> std::string;
private:
  BiomeType m_type;
  
  double m_minHeight;
  double m_maxHeight;

  double m_minTemp;
  double m_maxTemp;

  double m_minHumidity;
  double m_maxHumidity;
};

}

#endif // BIOME_H_
