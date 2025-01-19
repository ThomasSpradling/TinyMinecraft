#include "World/BlockFace.h"

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
  switch (direction) {
    case Direction::Front:  return {  0,  0,  1 };
    case Direction::Back:   return {  0,  0, -1 };
    case Direction::Left:   return { -1,  0,  0 };
    case Direction::Right:  return {  1,  0,  0 };
    case Direction::Top:    return {  0,  1,  0 };
    case Direction::Bottom: return {  0, -1,  0 };
    default:
      return { 0, 0, 0 };
  }
}
std::array<glm::vec3, 4> BlockFace::GetUnitVertices() const {
  switch (direction) {
    case Direction::Top:
      return { glm::vec3(0, 1, 0), glm::vec3(0, 1, 1),
               glm::vec3(1, 1, 1), glm::vec3(1, 1, 0) };
    case Direction::Bottom:
      return { glm::vec3(0, 0, 1), glm::vec3(0, 0, 0),
               glm::vec3(1, 0, 0), glm::vec3(1, 0, 1) };
    case Direction::Front:
      return { glm::vec3(0, 1, 1), glm::vec3(0, 0, 1),
               glm::vec3(1, 0, 1), glm::vec3(1, 1, 1) };
    case Direction::Back:
      return { glm::vec3(1, 1, 0), glm::vec3(1, 0, 0),
               glm::vec3(0, 0, 0), glm::vec3(0, 1, 0) };
    case Direction::Left:
      return { glm::vec3(0, 1, 0), glm::vec3(0, 0, 0),
               glm::vec3(0, 0, 1), glm::vec3(0, 1, 1) };
    case Direction::Right:
      return { glm::vec3(1, 1, 1), glm::vec3(1, 0, 1),
               glm::vec3(1, 0, 0), glm::vec3(1, 1, 0) };
    default:
      throw std::invalid_argument("Invalid face direction.");
  }
}

}
