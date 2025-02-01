#include "UI/UserInterface.h"
#include "Graphics/Renderer2D.h"
#include "Utils/defs.h"
#include "World/Biome.h"
#include <iomanip>
#include <sstream>

namespace UI {

void UserInterface::Draw() {
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

  debug << "Biome: "
        << World::Biome::GetBiomeName(m_biome) << "\n";

  Graphics::Renderer2D::DrawText(debug.str(), 15, 15, glm::vec4(1.0f));
}

void UserInterface::SetDebugValues(float temperature, float humidity, float continentalness, float erosion, float ridges, World::BiomeType biome) {
  m_temperature = temperature;
  m_humidity = humidity;
  m_erosion = erosion;
  m_continentalness = continentalness;
  m_ridges = ridges;

  m_biome = biome;
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
