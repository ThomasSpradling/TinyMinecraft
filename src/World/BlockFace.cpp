#include "World/BlockFace.h"
#include "Utils/Profiler.h"
#include "glm/fwd.hpp"
#include <iostream>

namespace World {

std::array<BlockFace, 6> BlockFace::allFaces {
  BlockFace::North,
  BlockFace::South,
  BlockFace::West,
  BlockFace::East,
  BlockFace::Up,
  BlockFace::Down
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

std::array<glm::vec3, 4> BlockFace::GetVertices(float width, float height) const {
  switch (direction) {
    case Direction::Up:
      return { glm::vec3(0, 1, 0), glm::vec3(0, 1, width), glm::vec3(height, 1, width), glm::vec3(height, 1, 0) };
    case Direction::Down:
      return { glm::vec3(0, 0, width), glm::vec3(0, 0, 0), glm::vec3(height, 0, 0), glm::vec3(height, 0, width) };
    case Direction::North:
      return { glm::vec3(0, height, 1), glm::vec3(0, 0, 1), glm::vec3(width, 0, 1), glm::vec3(width, height, 1) };
    case Direction::South:
      return { glm::vec3(width, height, 0), glm::vec3(width, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, height, 0) };
    case Direction::East:
      return { glm::vec3(1, height, width), glm::vec3(1, 0, width), glm::vec3(1, 0, 0), glm::vec3(1, height, 0) };
    case Direction::West:
      return { glm::vec3(0, height, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, width), glm::vec3(0, height, width) };
    default:
      throw std::invalid_argument("Invalid face direction.");
  }
}

std::array<glm::vec3, 4> BlockFace::GetFluidVertices(int depth, int maxDepth, Direction dir, bool floating) const {
  const float maxHeight = 0.8f;
  const float minHeight = 0.2f;


  float frontHeight, backHeight;
  
  float slope = (minHeight - maxHeight) / static_cast<float>(maxDepth - 1);
  if (depth == 0) {
    frontHeight = 0.8;
    backHeight = 0.8;
  } else if (floating) {
    frontHeight = 0.2;
    backHeight = 0.8;
  } else {
    frontHeight = slope * depth + maxHeight - slope;
    backHeight = slope * (depth + 1) + maxHeight - slope;
  }
  
  float southwestHeight = maxHeight;
  float southeastHeight = maxHeight;
  float northwestHeight = maxHeight;
  float northeastHeight = maxHeight;

  switch (dir) {
    case Direction::South:
      southwestHeight = backHeight;
      southeastHeight = backHeight;
      northwestHeight = frontHeight;
      northeastHeight = frontHeight;
      break;
    case Direction::North:
      southwestHeight = frontHeight;
      southeastHeight = frontHeight;
      northwestHeight = backHeight;
      northeastHeight = backHeight;
      break;
    case Direction::East:
      southwestHeight = backHeight;
      southeastHeight = frontHeight;
      northwestHeight = backHeight;
      northeastHeight = frontHeight;
      break;
    case Direction::West:
      southwestHeight = frontHeight;
      southeastHeight = backHeight;
      northwestHeight = frontHeight;
      northeastHeight = backHeight;
      break;
    default:
  }

  glm::vec3 southwestVert = glm::vec3(0.0f, southwestHeight, 0.0f);
  glm::vec3 northwestVert = glm::vec3(0.0f, northwestHeight, 1.0f);
  glm::vec3 northeastVert = glm::vec3(1.0f, northeastHeight, 1.0f);
  glm::vec3 southeastVert = glm::vec3(1.0f, southeastHeight, 0.0f);


  switch (direction) {
    case Direction::Up:
      return { southwestVert, northwestVert, northeastVert, southeastVert };
    case Direction::Down:
      return { glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f) };
    case Direction::North:
      return { northwestVert, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), northeastVert };
    case Direction::South:
      return { southeastVert, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), southwestVert };
    case Direction::East:
      return { northeastVert, glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), southeastVert };
    case Direction::West:
      return { southwestVert, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), northwestVert };
    default:
      throw std::invalid_argument("Invalid face direction.");
  }
}

}
