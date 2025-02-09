#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "Utils/mathgl.h"

namespace TinyMinecraft {

  namespace Geometry {

    enum class Face : uint8_t {
      Up,
      Down,
      East,
      West,
      North,
      South,
      None
    };

    struct Rectangle {
      float x;
      float y;
      float width;
      float height;

      Rectangle(float x, float y, float width, float height)
        : x(x)
        , y(y)
        , width(width)
        , height(height)
      {}

      Rectangle(const glm::vec2 &pos0, const glm::vec2 &pos1)
        : x(std::min(pos0.x, pos1.x))
        , y(std::min(pos0.y, pos1.y))
        , width(std::abs(pos0.x - pos1.x))
        , height(std::abs(pos0.y - pos1.y))
      {}
    };

    [[nodiscard]] auto GetNormal(Face face) -> glm::vec3;

    [[nodiscard]] auto GetVertices(Face face, float width = 1, float height = 1) -> std::array<glm::vec3, 4>;
    [[nodiscard]] auto GetFluidVertices(Face face, int depth = 0, int maxDepth = 7, Face direction = Face::None, bool floating = false) -> std::array<glm::vec3, 4>;

  }

}

#endif // GEOMETRY_H_
