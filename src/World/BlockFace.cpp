#include "World/BlockFace.h"

namespace World {

std::array<BlockFace, 6> BlockFace::allFaces {
  BlockFace::Direction::North,
  BlockFace::Direction::South,
  BlockFace::Direction::West,
  BlockFace::Direction::East,
  BlockFace::Direction::Up,
  BlockFace::Direction::Down
};

BlockFace::BlockFace(Direction face) : m_direction(face) {}

auto BlockFace::operator==(const BlockFace &other) const -> bool {
  return m_direction == other.m_direction;
}

auto BlockFace::GetNormal() const -> glm::vec3 {
  static const std::array<glm::ivec3, 6> normals = {{
    {  0,  0,  1 },
    {  0,  0, -1 },
    { -1,  0,  0 },
    {  1,  0,  0 },
    {  0,  1,  0 },
    {  0, -1,  0 } 
  }};

  return normals.at(static_cast<int>(m_direction));
}

auto BlockFace::GetVertices(float width, float height) const -> std::array<glm::vec3, 4> {
  switch (m_direction) {
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

auto BlockFace::GetFluidVertices(int depth, int maxDepth, Direction dir, bool floating) const -> std::array<glm::vec3, 4> {
  const float maxHeight = 0.8f;
  const float minHeight = 0.2f;


  float frontHeight = 0.0, backHeight = 0.0;
  
  float slope = (minHeight - maxHeight) / static_cast<float>(maxDepth - 1);
  if (depth == 0) {
    frontHeight = 0.8;
    backHeight = 0.8;
  } else if (floating) {
    frontHeight = 0.2;
    backHeight = 0.8;
  } else {
    frontHeight = slope * static_cast<float>(depth) + maxHeight - slope;
    backHeight = slope * (static_cast<float>(depth) + 1.f) + maxHeight - slope;
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


  switch (m_direction) {
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
