#ifndef BLOCK_FACE_H
#define BLOCK_FACE_H

#include "Utils/mathgl.h"

namespace World {

class BlockFace {
public:
  enum class Direction : uint8_t {
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

  auto operator==(const BlockFace &other) const -> bool;


  [[nodiscard]] auto GetNormal() const -> glm::vec3;

  /* Gets the vertices of this face based on its orientation counterclockwise from the top-left corner.
    - `width` will be the length along the x-axis for the front and back faces and the z-axis for the others.
    - `height` will be the length along the x-axis for the top and bottom faces and the y-axis for the others.
    This provides a useful and consistent orientation for greedy meshing.
  */
  [[nodiscard]] auto GetVertices(float width = 1.f, float height = 1.f) const -> std::array<glm::vec3, 4>;
  [[nodiscard]] auto GetFluidVertices(int depth = 0, int maxDepth = 7, Direction dir = Direction::None, bool floating = false) const -> std::array<glm::vec3, 4>;

private:
  Direction m_direction;
};

}

#endif // BLOCK_FACE_H
