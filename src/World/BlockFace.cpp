#include "World/BlockFace.h"
#include "Utils/Profiler.h"
#include "glm/fwd.hpp"

namespace World {

std::array<BlockFace, 6> BlockFace::allFaces {
  BlockFace::Front,
  BlockFace::Back,
  BlockFace::Left,
  BlockFace::Right,
  BlockFace::Top,
  BlockFace::Bottom
};

BlockFace::BlockFace(Direction face) : direction(face) {}

bool BlockFace::operator==(const BlockFace &other) const {
  return direction == other.direction;
}

glm::vec3 BlockFace::GetNormal() const {

  static const glm::vec3 normals[] = {
    {  0,  0,  1 },
    {  0,  0, -1 },
    { -1,  0,  0 },
    {  1,  0,  0 },
    {  0,  1,  0 },
    {  0, -1,  0 } 
  };

  return normals[static_cast<int>(direction)];
}

std::array<glm::vec3, 4> BlockFace::GetVertices(int width, int height) const {
  switch (direction) {
    case Direction::Top:
      return { glm::vec3(0, 1, 0), glm::vec3(0, 1, width), glm::vec3(height, 1, width), glm::vec3(height, 1, 0) };
    case Direction::Bottom:
      return { glm::vec3(0, 0, width), glm::vec3(0, 0, 0), glm::vec3(height, 0, 0), glm::vec3(height, 0, width) };
    case Direction::Front:
      return { glm::vec3(0, height, 1), glm::vec3(0, 0, 1), glm::vec3(width, 0, 1), glm::vec3(width, height, 1) };
    case Direction::Back:
      return { glm::vec3(width, height, 0), glm::vec3(width, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, height, 0) };
    case Direction::Right:
      return { glm::vec3(1, height, width), glm::vec3(1, 0, width), glm::vec3(1, 0, 0), glm::vec3(1, height, 0) };
    case Direction::Left:
      return { glm::vec3(0, height, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, width), glm::vec3(0, height, width) };
    default:
      throw std::invalid_argument("Invalid face direction.");
  }
}

}
