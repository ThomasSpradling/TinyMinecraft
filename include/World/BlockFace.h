#ifndef BLOCK_FACE_H
#define BLOCK_FACE_H

#include "Utils/mathgl.h"

namespace World {

class BlockFace {
public:
  enum Direction {
    North,  // +Z
    South,  // -Z
    West,   // -X
    East,   // +X
    Up,     // +Y
    Down,    // -Y
    None
  };
  static std::array<BlockFace, 6> allFaces;

  BlockFace() = default;
  BlockFace(Direction dir);

  ~BlockFace() = default;

  bool operator==(const BlockFace &other) const;

  glm::vec3 GetNormal() const;

  /* Gets the vertices of this face based on its orientation counterclockwise from the top-left corner.
    - `width` will be the length along the x-axis for the front and back faces and the z-axis for the others.
    - `height` will be the length along the x-axis for the top and bottom faces and the y-axis for the others.
    This provides a useful and consistent orientation for greedy meshing.
  */
  std::array<glm::vec3, 4> GetVertices(float width = 1.f, float height = 1.f) const;
  std::array<glm::vec3, 4> GetFluidVertices(int depth = 0, int maxDepth = 7, Direction dir = None, bool floating = false) const;

private:
  Direction direction;
};

}

#endif // BLOCK_FACE_H
