#ifndef NOISE_H_
#define NOISE_H_

#include <PerlinNoise/PerlinNoise.h>

namespace Utils {

double OctaveNoise(double x, double z, PerlinNoise &perlin, int octaves = 5, double persistence = 0.5, double lacunarity = 2);
double GenerateDomainNoise(double x, double z, PerlinNoise &perlin,  int octaves = 5, double persistence = 0.5, double lacunarity = 2);

}

#endif // NOISE_H_
