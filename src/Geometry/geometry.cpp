#include "Geometry/geometry.h"
#include "Utils/Logger.h"

namespace TinyMinecraft {

  namespace Geometry {

    auto GetNormal(Face face) -> glm::vec3 {
      switch (face) {
        case Face::Up:
          return { 0, 1, 0 };
        case Face::Down:
          return { 0, -1, 0 };
        case Face::East:
          return { 1, 0, 0 };
        case Face::West:
          return { -1, 0, 0 };
        case Face::North:
          return { 0, 0, 1 };
        case Face::South:
          return { 0, 0, -1 };
        default:
          break;
      }

      Utils::Logger::Warning("Invalid face normal.");
      return { 0, 0, 0 };
    }

    auto GetVertices(Face face, float width, float height) -> std::array<glm::vec3, 4> {
      switch (face) {
        case Face::Up:
          return { glm::vec3(0, 1, 0), glm::vec3(0, 1, width), glm::vec3(height, 1, width), glm::vec3(height, 1, 0) };
        case Face::Down:
          return { glm::vec3(0, 0, width), glm::vec3(0, 0, 0), glm::vec3(height, 0, 0), glm::vec3(height, 0, width) };
        case Face::North:
          return { glm::vec3(0, height, 1), glm::vec3(0, 0, 1), glm::vec3(width, 0, 1), glm::vec3(width, height, 1) };
        case Face::South:
          return { glm::vec3(width, height, 0), glm::vec3(width, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, height, 0) };
        case Face::East:
          return { glm::vec3(1, height, width), glm::vec3(1, 0, width), glm::vec3(1, 0, 0), glm::vec3(1, height, 0) };
        case Face::West:
          return { glm::vec3(0, height, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, width), glm::vec3(0, height, width) };
        default:
          throw std::invalid_argument("Invalid face direction.");
      }
    }

    auto GetFluidVertices(Face face, int depth, int maxDepth, Face direction, bool floating) -> std::array<glm::vec3, 4> {
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

      switch (direction) {
        case Face::South:
          southwestHeight = backHeight;
          southeastHeight = backHeight;
          northwestHeight = frontHeight;
          northeastHeight = frontHeight;
          break;
        case Face::North:
          southwestHeight = frontHeight;
          southeastHeight = frontHeight;
          northwestHeight = backHeight;
          northeastHeight = backHeight;
          break;
        case Face::East:
          southwestHeight = backHeight;
          southeastHeight = frontHeight;
          northwestHeight = backHeight;
          northeastHeight = frontHeight;
          break;
        case Face::West:
          southwestHeight = frontHeight;
          southeastHeight = backHeight;
          northwestHeight = frontHeight;
          northeastHeight = backHeight;
          break;
        default:
          break;
      }

      glm::vec3 southwestVert = glm::vec3(0.0f, southwestHeight, 0.0f);
      glm::vec3 northwestVert = glm::vec3(0.0f, northwestHeight, 1.0f);
      glm::vec3 northeastVert = glm::vec3(1.0f, northeastHeight, 1.0f);
      glm::vec3 southeastVert = glm::vec3(1.0f, southeastHeight, 0.0f);


      switch (face) {
        case Face::Up:
          return { southwestVert, northwestVert, northeastVert, southeastVert };
        case Face::Down:
          return { glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f) };
        case Face::North:
          return { northwestVert, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), northeastVert };
        case Face::South:
          return { southeastVert, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), southwestVert };
        case Face::East:
          return { northeastVert, glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), southeastVert };
        case Face::West:
          return { southwestVert, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), northwestVert };
        default:
          throw std::invalid_argument("Invalid face direction.");
      }
    }

  }

}
