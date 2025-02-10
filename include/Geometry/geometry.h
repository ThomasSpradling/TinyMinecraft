#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "Utils/Printable.h"
#include "Utils/mathgl.h"
#include <limits>
#include <sstream>

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

    struct Rectangle : public Utils::Printable {
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

      auto ToString() const -> std::string override {
        std::stringstream ss;
        ss << "Rectangle(";
        ss << "x: " << x << ", ";
        ss << "y: " << y << ", ";
        ss << "width: " << width << ", ";
        ss << "height: " << height << ")";
        return ss.str();
      }
    };

    struct Ray : public Utils::Printable {
      glm::vec3 origin { 0.0f };
      glm::vec3 direction { 0.0f };
      float maxRayDistance;

      Ray(const glm::vec3 &origin, const glm::vec3 &direction, float maxRayDistance = std::numeric_limits<float>::infinity())
        : origin(origin)
        , direction(direction)
        , maxRayDistance(maxRayDistance)
      {}
      
      auto ToString() const -> std::string override {
        std::stringstream ss;
        ss << "Ray(";
        ss << "origin: " << origin << ", ";
        ss << "direction: " << direction << ", ";
        ss << "max distance: " << maxRayDistance << ")";
        return ss.str();
      }
    };

    [[nodiscard]] auto GetNormal(Face face) -> glm::vec3;

    [[nodiscard]] auto GetVertices(Face face, float width = 1.0f, float height = 1.0f) -> std::array<glm::vec3, 4>;
    [[nodiscard]] auto GetFluidVertices(Face face, int depth = 0, int maxDepth = 7, Face direction = Face::None, bool floating = false) -> std::array<glm::vec3, 4>;

  }

  auto operator<<(std::ostream &os, Geometry::Face face) noexcept -> std::ostream &;

}

#endif // GEOMETRY_H_
