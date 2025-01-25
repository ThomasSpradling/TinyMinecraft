#ifndef BIOME_H_
#define BIOME_H_

#include "Utils/noise.h"
#include "World/Block.h"
namespace World {

enum class BiomeType {
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

  bool IsValid(float temperature, float humidity) const;
  float GenerateHeight(int x, int z, PerlinNoise &heightMap) const;
  Block GenerateBlock(int x, int y, int z, int height, float stoneMap) const;

  BiomeType GetType() const;

  float GetMinHeight() const;
  float GetMaxHeight() const;

  float GetMinTemperature() const;
  float GetMaxTemperature() const;

  float GetMinHumidity() const;
  float GetMaxHumidty() const;

  static std::string GetBiomeName(BiomeType type);
private:
  BiomeType type;
  
  float minHeight;
  float maxHeight;

  float minTemp;
  float maxTemp;

  float minHumidity;
  float maxHumidity;
};

}

#endif // BIOME_H_
