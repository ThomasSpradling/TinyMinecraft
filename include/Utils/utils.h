#ifndef UTILS_H_
#define UTILS_H_

#include <array>
#include <Utils/mathgl.h>

namespace Utils {

// Linearly scales a value from starting range to end range
float ScaleValue(float min1, float max1, float min2, float max2, float value);

// Face
template <int VertCount>
glm::vec3 CalculateConvexCenter(const std::array<glm::vec3, VertCount> &vertices) {
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
