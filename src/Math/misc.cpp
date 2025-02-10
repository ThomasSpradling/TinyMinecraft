#include "Math/misc.h"
#include "Geometry/geometry.h"
#include "Utils/Logger.h"
#include <limits>

namespace TinyMinecraft {

  auto GetRayGridInteresction(const Geometry::Ray &ray) -> BlockInteresction {
  if (ray.origin == glm::vec3(0.0f)) {
    Utils::Logger::Warning("Gathering direction of 0 ray!");
  }

  constexpr float inf = std::numeric_limits<float>::infinity();

  glm::vec3 origin = ray.origin;
  glm::vec3 direction = glm::normalize(ray.direction);

  constexpr auto computeDistance = [&](float o, float r) -> float {
    if (r > 0.0f)
      return (glm::floor(o) + 1.0f - o) / r;
    else if (r < 0.0f) {
      float fraction = o - glm::floor(o);
      return (fraction == 0.0f) ? (1.0f / (-r)) : (fraction / (-r));
    } else
      return inf;
  };
    
  constexpr auto sign = [](float x) -> int {
    return (x > 0.0f) - (x < 0.0f);
  };

  const glm::vec3 t {
    computeDistance(origin.x, direction.x),
    computeDistance(origin.y, direction.y),
    computeDistance(origin.z, direction.z)
  };

  int axis = 0;
  float tmin = t.x;
  if (t.y < tmin) {
    tmin = t.y;
    axis = 1;
  }
  if (t.z < tmin) {
    tmin = t.z;
    axis = 2;
  }

  glm::vec3 result = origin + tmin * direction;

  glm::vec3 targetedBlock = glm::floor(origin);
  Geometry::Face face = Geometry::Face::None;

  if (axis == 0) {
    int s = sign(direction.x);
    targetedBlock.x += s;
    face = (s > 0) ? Geometry::Face::West : Geometry::Face::East;
  } else if (axis == 1) {
    int s = sign(direction.y);
    targetedBlock.y += s;
    face = (s > 0) ? Geometry::Face::Down : Geometry::Face::Up;
  } else {
    int s = sign(direction.z);
    targetedBlock.z += s;
    face = (s > 0) ? Geometry::Face::South : Geometry::Face::North;
  }

  return BlockInteresction{ result, targetedBlock, face, tmin };
}


}