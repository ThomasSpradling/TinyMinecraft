#include "Application/Game.h"
#include "Application/InputHandler.h"
#include "Entity/Player.h"
#include "Entity/PlayerController.h"
#include "Geometry/geometry.h"
#include "Graphics/Renderer.h"
#include "Graphics/Renderer2D.h"
#include "Graphics/WireframeRenderer.h"
#include "Scene/PlayerCameras.h"
#include "Utils/defs.h"
#include "Utils/Profiler.h"
#include "World/Biome.h"
#include "World/Block.h"
#include "World/BlockType.h"
#include "World/World.h"
#include <chrono>
#include <algorithm>
#include <memory>
#include <ratio>

namespace TinyMinecraft {

  namespace Application {
    
    Game::Game()
      : m_window(viewportWidth, viewportHeight, "Look mum! I made some chunks!")
      , m_renderer(viewportWidth, viewportHeight)
      , m_ortho(0, viewportWidth, viewportHeight, 0)
      , m_player(std::make_shared<Entity::PlayerController>())
    {
      PROFILE_FUNCTION(Game)

      const float fov = 45.f;
      World::BlockData::Initialize();

      m_world = std::make_shared<World::World>();

      // TODO: Replace with player orientations
      m_camera = std::make_unique<Scene::FirstPersonPlayerCamera>(
        fov,
        static_cast<float>(m_window.GetWidth()) / static_cast<float>(m_window.GetHeight()),
        0.1f,
        2048.0f
      );
      m_player.SetWorld(m_world);

      InputHandler::Initialize();
    }

    void Game::Run() {
      using clock = std::chrono::high_resolution_clock;

      auto previousTime = clock::now();
      double lag = 0.0;

      const double maxWaitTime = 250.f; // ms

    #ifdef UTILS_ShowFPS
      int frameCount = 0;
      double fpsTimer = 0.0;
      int fps = 0;
      const double displayFPSInterval = 1000.f; // ms
    #endif

      while (!m_window.ShouldClose()) {
        m_window.PollEvents();

        auto currentTime = clock::now();

        std::chrono::duration<double, std::milli> elapsed = currentTime - previousTime;
        previousTime = currentTime;

        double frameTime = std::clamp(elapsed.count(), 0.0, maxWaitTime);
        lag += frameTime;

        // try to catch back up
        while (lag >= FIXED_UPDATE_INTERVAL) {
          Update();
          lag -= FIXED_UPDATE_INTERVAL;
        }
        
        Render(lag / FIXED_UPDATE_INTERVAL);

    #ifdef UTILS_ShowFPS
        frameCount++;
        fpsTimer += frameTime;
        if (fpsTimer >= displayFPSInterval) {
          fps = frameCount;
          frameCount = 0;
          fpsTimer -= displayFPSInterval;

          m_ui.SetCurrentFPS(fps);
        }
    #endif

      }
    }

    void Game::Update() {
      PROFILE_FUNCTION(Game)

      m_player.Update();
      // m_world->HandlePlayerMovement(before, after);
      
      glm::vec3 pos = m_player.GetPosition();

      m_ui.SetPlayerPosition(pos);
      m_ui.SetChunkPosition(m_world->GetChunkPosFromCoords(pos));
      m_ui.SetDebugValues(
        m_world->GetTemperature(pos.x, pos.z),
        m_world->GetHumidity(pos.x, pos.z),
        m_world->GetContinentalness(pos.x, pos.z),
        m_world->GetErosion(pos.x, pos.z),
        m_world->GetRidges(pos.x, pos.z),
        World::BiomeType::Desert,
        m_player.GetTargetingBlock()
        // m_world->GetBiome(pos.x, pos.z)
      );

      m_world->Update(m_player.GetPosition());
    }

    void Game::Render(double) {
      PROFILE_FUNCTION(Game)

      // m_ui.Arrange();
      m_renderer.ClearBackground(glm::vec3(0.0f));


      m_renderer.Begin3D(m_camera);

      if (InputHandler::IsKeyPressed(GLFW_KEY_TAB)) {
        m_renderer.ToggleWireframeMode();
      }

    #ifdef GFX_ShadowMapping
        m_renderer.RenderShadows(m_world);
    #endif

        m_renderer.RenderWorld(*m_world);

      m_renderer.End3D();

      auto [location, face, block] = m_player.GetTargetingBlock();
      if (face != Geometry::Face::None && block != BlockType::Air) {
        Graphics::WireframeRenderer::DrawBlock(location, glm::vec4(1.0f, 1.0f, 1.0f, 0.8f));
      }

      // TODO: Move to debug tools class
      // glm::vec3 pos = m_player.GetPosition();
      // Graphics::WireframeRenderer::DrawChunk(m_world->GetChunkPosFromCoords(pos), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

      Graphics::WireframeRenderer::Render(m_camera);
      
      Graphics::Renderer2D::BeginScene(m_ortho);
        m_renderer.RenderUI(m_ui);
      Graphics::Renderer2D::EndScene();

      m_window.SwapBuffers();
    }

  }

}
