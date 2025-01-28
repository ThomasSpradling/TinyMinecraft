#ifndef UTILS_H_
#define UTILS_H_

#include <array>
#include <Utils/mathgl.h>

namespace Utils {

// Linearly scales a value from starting range to end range
template <typename T>
auto ScaleValue(T min1, T max1, T min2, T max2, T value) -> T {
  T slope = (max2 - min2) / (max1 - min1);
  T intercept = min2 - slope * min1;
  return slope * value + intercept;
}

// Face
template <int VertCount>
auto CalculateConvexCenter(const std::array<glm::vec3, VertCount> &vertices) -> glm::vec3 {
  glm::vec3 res { 0.0f };
  float weights = 0.0;

  for (int i = 1; i < VertCount - 1; ++i) {
    float weight = glm::length(vertices[i] - vertices[i + 1]) + glm::length(vertices[i] - vertices[i - 1]);
    res += vertices[i] * weight;
    weights += weight;
  }

  return res / weights;
}

}

#endif // UTILS_H_
