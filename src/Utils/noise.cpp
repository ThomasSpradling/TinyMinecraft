#include "Utils/noise.h"

namespace Utils {

double OctaveNoise(double x, double z, PerlinNoise &perlin, int octaves, double persistence, double lacunarity) {
  double total = 0.0;
  double frequency = 1.0;
  double amplitude = 1.0;
  double maxValue = 0.0;

  for (int i = 0; i < octaves; ++i) {
    total += perlin.noise(x * frequency, 13.5, z * frequency) * amplitude;
    maxValue += amplitude;

    amplitude *= persistence;
    frequency *= lacunarity;
  }

  return total / maxValue;
}

double GenerateDomainNoise(double x, double z, PerlinNoise &perlin,  int octaves, double persistence, double lacunarity) {
  double offset = OctaveNoise(x, z, perlin, octaves, persistence, lacunarity) * 20;
  // double offsetY = OctaveNoise(x, z, perlin, octaves, persistence, lacunarity) * 20;

  return OctaveNoise(x + offset, z + offset, perlin, octaves, persistence, lacunarity);
}

}

