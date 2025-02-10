#ifndef MISCELLANEOUS_MATH_H_
#define MISCELLANEOUS_MATH_H_

#include "Geometry/geometry.h"
#include "Utils/Printable.h"
#include "Utils/mathgl.h"
#include <sstream>

namespace TinyMinecraft {

  struct BlockInteresction : public Utils::Printable {
    glm::vec3 location;
    glm::vec3 blockPosition;
    Geometry::Face face;
    float distance;

    BlockInteresction(glm::vec3 &location, glm::vec3 &blockPos, Geometry::Face face, float distance)
      : location(location)
      , blockPosition(blockPos)
      , face(face)
      , distance(distance)
    {}

    auto ToString() const -> std::string override {
      std::stringstream ss;
      ss << "Interection(";
      ss << "loc: " << location << ", ";
      ss << "block: " << blockPosition;
      ss << "face: " << face;
      ss << "distance: " << distance;
      ss << ")";
      return ss.str();
    }
  };

  /* Given point `ray.origin`, finds the nearest point on a face within the unit cube lattice in the direction of `ray.direction`. */
  [[nodiscard]] auto GetRayGridInteresction(const Geometry::Ray &ray) -> BlockInteresction;
}

#endif // MISCELLANEOUS_MATH_H_
