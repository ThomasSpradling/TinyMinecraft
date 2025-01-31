#ifndef NOISE_MANAGER_H_
#define NOISE_MANAGER_H_

#include "FastNoiseLite.h"
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <vector>
#include "nlohmann/json.hpp"
#include "Utils/mathgl.h"

namespace Math {

enum class Noise {
  Temperature,
  Humidity,
  Stone,
  Tree,
  Continentalness,
  Erosion,
  Ridges,
  Grass
};

class NoiseManager {
public:
  static void Initialize(int seed);

  static auto GetImprovedSimplexNoise(Noise type, const glm::vec2 &point) -> double;
  static auto GetImprovedSimplexNoise(Noise type, const glm::vec3 &point) -> double;
private:
  static auto GetImprovedSimplexNoise(Noise type, const std::initializer_list<float> &args) -> double;
  static auto GenerateNewSeed(int seed);
  static auto GetDimensions(Noise type) -> int;

  static auto NoiseToString(Noise type) -> std::string;
  static auto StringToNoise(const std::string &type) -> Noise;

  static FastNoiseLite s_noise;
  static std::unordered_map<Noise, std::vector<double>> s_amplitudes;
  static std::unordered_map<Noise, int> s_octaveOffsets;
  static std::unordered_map<Noise, int> s_seeds;
};

}

#endif // NOISE_MANAGER_H_
