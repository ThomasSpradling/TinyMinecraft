#ifndef BLOCK_FACE_H
#define BLOCK_FACE_H

#include "Utils/mathgl.h"

namespace World {

class BlockFace {
public:
  enum Direction {
    Front,  // +Z
    Back,   // -Z
    Left,   // -X
    Right,  // +X
    Top,    // +Y
    Bottom  // -Y
  };
  static std::array<BlockFace, 6> allFaces;

  BlockFace() = default;
  BlockFace(Direction dir);

  ~BlockFace() = default;

  bool operator==(const BlockFace &other) const;

  glm::vec3 GetNormal() const;
  std::array<glm::vec3, 4> GetUnitVertices() const;

private:
  Direction direction;
};

}

#endif // BLOCK_FACE_H
