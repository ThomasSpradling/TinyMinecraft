#include "Application/Game.h"
#include "Application/InputHandler.h"
#include "Graphics/Renderer.h"
#include "Graphics/Renderer2D.h"
#include "Utils/defs.h"
#include "Utils/Profiler.h"
#include "World/Biome.h"
#include <chrono>
#include <algorithm>
#include <ratio>
#include <unistd.h>

namespace Application {
Game::Game()
  : m_window(viewportWidth, viewportHeight, "Look mum! I made some chunks!", m_inputHandler)
  , m_renderer(viewportWidth, viewportHeight)
  , m_ortho(0, viewportWidth, viewportHeight, 0)
{
  PROFILE_FUNCTION(Game)

  const float fov = 45.f;
    
  // TODO: Replace with player orientations
  m_camera = std::make_shared<Scene::Camera>();
  m_camera->SetPosition(glm::vec3(0.f, 100.f, 0.f));
  m_camera->SetFOV(fov);
  m_camera->SetAspectRatio(static_cast<float>(m_window.GetWidth()) / static_cast<float>(m_window.GetHeight()));
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
    ProcessInput();

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

void Game::ProcessInput() {
  const float maxPitch = 89.f;

  m_window.PollEvents();

  if (m_inputHandler.IsKeyPressed(GLFW_KEY_ESCAPE)) {
    m_window.Close();
  }

  // Camera keyboard
  glm::vec3 cameraDirection { 0.f };
  if (m_inputHandler.IsKeyDown(GLFW_KEY_W))
    cameraDirection += m_camera->GetFront();
  if (m_inputHandler.IsKeyDown(GLFW_KEY_S))
    cameraDirection -= m_camera->GetFront();
  if (m_inputHandler.IsKeyDown(GLFW_KEY_D))
    cameraDirection += m_camera->GetRight();
  if (m_inputHandler.IsKeyDown(GLFW_KEY_A))
    cameraDirection -= m_camera->GetRight();

  if (m_inputHandler.IsKeyDown(GLFW_KEY_LEFT_SHIFT))
    cameraDirection -= glm::vec3(0.0f, 1.0f, 0.0f);
  
  if (m_inputHandler.IsKeyDown(GLFW_KEY_SPACE))
    cameraDirection += glm::vec3(0.0f, 1.0f, 0.0f);

  m_camera->SetMoveDirection(cameraDirection);

  // Camera mouse

  float yaw = m_camera->GetYaw();
  yaw += m_inputHandler.GetMouseDeltaX();
  
  float pitch = m_camera->GetPitch();
  pitch -= m_inputHandler.GetMouseDeltaY();
  pitch = std::clamp(pitch, -maxPitch, maxPitch);

  m_camera->UpdateViewDirection(yaw, pitch);
}

void Game::Update() {
  PROFILE_FUNCTION(Game)

  glm::ivec3 before = m_camera->GetPosition();
  m_camera->Move();
  glm::ivec3 after = m_camera->GetPosition();
  m_world.HandlePlayerMovement(before, after);

  glm::ivec3 pos = m_camera->GetPosition();

  m_ui.SetPlayerPosition(pos);
  m_ui.SetChunkPosition(m_world.GetChunkPosFromCoords(pos));
  m_ui.SetDebugValues(
    m_world.GetTemperature(pos.x, pos.z),
    m_world.GetHumidity(pos.x, pos.z),
    m_world.GetContinentalness(pos.x, pos.z),
    m_world.GetErosion(pos.x, pos.z),
    m_world.GetRidges(pos.x, pos.z),
    World::BiomeType::Desert
    // m_world.GetBiome(pos.x, pos.z)
  );
}

void Game::Render(double) {
  PROFILE_FUNCTION(Game)

  m_world.Update(m_camera->GetPosition());

  // m_ui.Arrange();
  m_renderer.ClearBackground(glm::vec3(0.0f));


  m_renderer.Begin3D(m_camera);

    if (m_inputHandler.IsKeyPressed(GLFW_KEY_TAB)) {
      m_renderer.ToggleWireframeMode();
    }

#ifdef GFX_ShadowMapping
    m_renderer.RenderShadows(m_world);
#endif

    m_renderer.RenderWorld(m_world);

  m_renderer.End3D();
  
  Graphics::Renderer2D::BeginScene(m_ortho);
   m_renderer.RenderUI(m_ui);
  Graphics::Renderer2D::EndScene();

  m_window.SwapBuffers();
}

}
