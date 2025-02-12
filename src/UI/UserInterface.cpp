#include "UI/UserInterface.h"
#include "Geometry/geometry.h"
#include "Graphics/Renderer2D.h"
#include "Utils/defs.h"
#include "World/Biome.h"
#include "World/World.h"
#include <iomanip>
#include <sstream>

namespace TinyMinecraft {

  namespace UI {

    void UserInterface::Draw() {
      DrawDebug();
      DrawCrosshair();
    }

    void UserInterface::DrawDebug() {
      std::ostringstream debug;

#ifdef UTILS_ShowFPS  // FPS
      debug << "CURRENT FPS: " << m_currentFPS << "\n";
#endif

      debug << std::fixed << std::setprecision(3);
      debug << "XYZ: "
            << m_playerPosition.x << ", " 
            << m_playerPosition.y << ", "
            << m_playerPosition.z << "\n";
      debug << std::defaultfloat;

      debug << "Chunk: "
            << m_chunkPosition.x << ", "
            << m_chunkPosition.y << "\n\n";

      debug << std::fixed << std::setprecision(5);
      debug << "Environment: "
            << "T " << m_temperature << ", H " << m_humidity << ", C " << m_continentalness << ", E " << m_erosion << ", R " << m_ridges << "\n";
      debug << std::defaultfloat;

      debug << "Biome: "
            << World::Biome::GetBiomeName(m_biome) << "\n";

      auto [location, face, block] = m_targetingBlock;
      if (face != Geometry::Face::None && block) {
        debug << "Looking At: "
              << face << " face of " << block << "\n";
      } else {
        debug << "\n";
      }

      Graphics::Renderer2D::DrawString(debug.str(), 15, 15, glm::vec4(1.0f), 1.0f);
    }

    void UserInterface::DrawCrosshair() {
      const auto drawPivotedRectangle = [](glm::vec2 center, glm::vec2 size, glm::vec4 color) {
        glm::vec2 position = center - size / 2.0f;
        Graphics::Renderer2D::DrawRectangle(Geometry::Rectangle{position.x, position.y, size.x, size.y}, color);
      };

      glm::vec4 color = glm::vec4(1.0f);
      glm::vec2 center = glm::vec2(viewportWidth, viewportHeight) / 2.0f;
      drawPivotedRectangle(center, glm::vec2(5.0f, 25.0f), color);
      drawPivotedRectangle(center, glm::vec2(25.0f, 5.0f), color);
    }

    void UserInterface::SetDebugValues(float temperature, float humidity, float continentalness, float erosion, float ridges, World::BiomeType biome, World::BlockLocation targetBlock) {
      m_temperature = temperature;
      m_humidity = humidity;
      m_erosion = erosion;
      m_continentalness = continentalness;
      m_ridges = ridges;
      m_biome = biome;
      m_targetingBlock = targetBlock;
    }

    void UserInterface::SetCurrentFPS(int fps) {
      m_currentFPS = fps;
    }

    void UserInterface::SetPlayerPosition(const glm::vec3 &pos) {
      m_playerPosition = pos;
    }

    void UserInterface::SetChunkPosition(const glm::ivec2 &pos) {
      m_chunkPosition = pos;
    }

  }

}
