#ifndef USER_INTERFACE_H_
#define USER_INTERFACE_H_

#include "World/Biome.h"
#include "World/World.h"

namespace TinyMinecraft {

  namespace UI {

    class UserInterface {
    public:
      UserInterface() = default;
      
      void Draw();
      void DrawDebug();
      void DrawCrosshair();
      
      void SetCurrentFPS(int fps);
      void SetPlayerPosition(const glm::vec3 &pos);
      void SetChunkPosition(const glm::ivec2 &pos);
      void SetDebugValues(float temperature, float humidity, float continentalness, float erosion, float ridges, World::BiomeType biome, World::BlockLocation targetBlock);
    private:
      size_t m_basicVertexCount = 0, m_textVertexCount = 0;

      // debug
      int m_currentFPS = 0;
      glm::vec3 m_playerPosition { 0.f };
      glm::ivec2 m_chunkPosition { 0 };

      static constexpr int viewportWidth = 1920;
      static constexpr int viewportHeight = 1080;

      float m_temperature = 0, m_humidity = 0, m_continentalness = 0, m_erosion = 0, m_ridges = 0;
      World::BiomeType m_biome { World::BiomeType::Grassland };
      World::BlockLocation m_targetingBlock;
    };

  }

}

#endif // USER_INTERFACE_H_
