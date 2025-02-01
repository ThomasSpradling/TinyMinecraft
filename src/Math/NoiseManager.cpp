// #include "Math/NoiseManager.h"
// #include "FastNoise/Generators/Modifiers.h"
// #include "Math/seed.h"
// #include "Utils/Logger.h"
// #include "Utils/Profiler.h"
// #include "nlohmann/json.hpp"
// #include <cmath>
// #include <fstream>

// namespace Math {

// // FastNoiseLite NoiseManager::s_noise;
// std::unordered_map<Noise, std::vector<double>> NoiseManager::s_amplitudes;
// std::unordered_map<Noise, int> NoiseManager::s_octaveOffsets;
// std::unordered_map<Noise, int> NoiseManager::s_seeds;
// // s_noise.Set

// void NoiseManager::Initialize(int seed) {
//   // auto noise = FastNoise::New<FastNoise::Perlin>();
//   // noise.

//   // s_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
//   // FastNoise::SeedOffset offset;
//   // auto seedOffset = FastNoise::New<FastNoise::SeedOffset>();
//   // seedOffset->SetOffset(1);
//   // seedOffset->SetSource(noise);

//   std::ifstream file("../data/noise.json");
//   if (!file.is_open()) {
//     Utils::g_logger.Error("noise: file cannot open");
//     exit(1);
//   }

//   nlohmann::json noiseData;
//   file >> noiseData;
  
//   std::cout << noiseData << std::endl;

//   int newSeed = seed;

//   for (const auto &[name, parameters] : noiseData["noise_maps"].items()) {
//     Noise noise = StringToNoise(name);

//     for (const auto &amplitude : parameters["amplitude"]) {
//       s_amplitudes[noise].push_back(amplitude);
//     }

//     s_octaveOffsets[noise] = parameters["octave_offset"];

//     s_seeds[noise] = newSeed;

//     // generates fresh seed deterministically
//     newSeed = TwistSeedFNV1a(newSeed);
//   }
// }

// auto NoiseManager::GetImprovedSimplexNoise(Noise type, const glm::vec2 &point) -> double {
//   PROFILE_FUNCTION_Misc()

//   return GetImprovedSimplexNoise(type, { point.x, point.y });
// }

// auto NoiseManager::GetImprovedSimplexNoise(Noise type, const glm::vec3 &point) -> double {
//   return GetImprovedSimplexNoise(type, { point.x, point.y, point.z });
// }


// auto NoiseManager::GetImprovedSimplexNoise(Noise type, const std::initializer_list<float> &args) -> double {
//   if (args.size() != GetDimensions(type)) {
//     Utils::g_logger.Error("Noise: Incorrect dimension! Expected {} dimensions for Noise::{}.",
//       GetDimensions(type),
//       NoiseToString(type));
//     exit(1);
//   }

//   PROFILE_FUNCTION_Misc()

//   int dimensions = args.size();
  
//   const std::vector<double> &amplitudes = s_amplitudes[type];
//   const int &offset = s_octaveOffsets[type];

//   int seed = s_seeds[type];

//   int octavesCount = amplitudes.size();
//   int totalActive = 0;

//   std::vector<float> coords(args);

//   float total = 0.0f;
//   float normalizationFactor = 0.0f;

//   for (double amplitude : amplitudes)
//     if (amplitude != 0.0) totalActive++;

//   // s_fnSimplex.
//   for (int i = 0; i < octavesCount; ++i) {
//     // seed = TwistSeedLCG(seed);

//     double frequency = offset >= 1 ? 1.0 / (1 << (offset - 1)) : (1 << (1 - offset));
//     double amplitude = 1.04 * amplitudes[i] * (1 << (octavesCount - i - 1)) / ((1 << octavesCount) - 1);

//     double noiseVal;
//     if (coords.size() == 3) {
//       noiseVal = s_noise.GetNoise(coords[0] * frequency, coords[1] * frequency, coords[2] * frequency);
//     } else if (coords.size() == 2) {
//       noiseVal = s_noise.GetNoise(coords[0] * frequency, coords[1] * frequency);
//     }
//     noiseVal *= amplitude;

//     total += noiseVal;
//     normalizationFactor += amplitude;
//   }

//   return total / (2.0f * normalizationFactor) + 0.5f;
// }

// auto NoiseManager::GetDimensions(Noise type) -> int {
//   switch (type) {
//     case Noise::Density:
//       return 3;
//     case Noise::Stone:
//     case Noise::Tree:
//     case Noise::Continentalness:
//     case Noise::Erosion:
//     case Noise::Humidity:
//     case Noise::Temperature:
//     case Noise::Ridges:
//     case Noise::Grass:
//     default:
//       return 2;
//   }
// }

// auto NoiseManager::NoiseToString(Noise type) -> std::string {
//   switch (type) {
//     case Noise::Stone:
//       return "stone";
//     case Noise::Tree:
//       return "tree";
//     case Noise::Grass:
//       return "grass";
//     case Noise::Continentalness:
//       return "continentalness";
//     case Noise::Erosion:
//       return "erosion";
//     case Noise::Humidity:
//       return "humidity";
//     case Noise::Temperature:
//       return "temperature";
//     case Noise::Ridges:
//       return "ridges";
//     case Noise::Density:
//       return "density";
//   }
// }

// auto NoiseManager::StringToNoise(const std::string &type) -> Noise {
//   if (type == "stone")
//     return Noise::Stone;
//   if (type == "tree")
//     return Noise::Tree;
//   if (type == "grass")
//     return Noise::Grass;
//   if (type == "continentalness")
//     return Noise::Continentalness;
//   if (type == "erosion")
//     return Noise::Erosion;
//   if (type == "humidity")
//     return Noise::Humidity;
//   if (type == "stone")
//     return Noise::Stone;
//   if (type == "temperature")
//     return Noise::Temperature;
//   if (type == "ridges")
//     return Noise::Ridges;
//   if (type == "density")
//     return Noise::Density;

//   Utils::g_logger.Error("Invalid noise name \"{}\".", type);
//   exit(1);
// }

// }
