#include "Application/Game.h"
#include "Application/InputHandler.h"
#include "Utils/defs.h"
#include "Utils/Profiler.h"
#include <chrono>
#include <algorithm>
#include <ratio>
#include <unistd.h>

namespace Application {

void Game::Initialize() {
  PROFILE_FUNCTION(Game)

  constexpr int viewportWidth = 1920;
  constexpr int viewportHeight = 1080;
  // constexpr int viewportWidth = 600;
  // constexpr int viewportHeight = 400;

  window.Initialize(viewportWidth, viewportHeight, "Look mum! I made some chunks!", inputHandler);
  renderer.Initialize(viewportWidth, viewportHeight);
  world.Initialize();
  ui.Initialize();
  
  // TODO: Replace with player orientations
  camera = std::make_shared<Scene::Camera>();
  camera->SetPosition(glm::vec3(0.f, 100.f, 0.f));
  camera->SetFOV(45.f);
  camera->SetAspectRatio(static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight()));
}

void Game::Run() {
  using clock = std::chrono::high_resolution_clock;

  auto previousTime = clock::now();
  double lag = 0.0;

#ifdef UTILS_ShowFPS
  int frameCount = 0;
  double fpsTimer = 0.0;
  int fps = 0;
#endif

  while (!window.ShouldClose()) {
    ProcessInput();

    auto currentTime = clock::now();

    std::chrono::duration<double, std::milli> elapsed = currentTime - previousTime;
    previousTime = currentTime;

    double frameTime = std::clamp(elapsed.count(), 0.0, 250.0);
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
    if (fpsTimer >= 1000.0f) {
      fps = frameCount;
      frameCount = 0;
      fpsTimer -= 1000.0f;

      ui.SetCurrentFPS(fps);
    }
#endif

  }
}

void Game::Shutdown() {
  glfwTerminate();
}

// Private Methods

void Game::ProcessInput() {
  window.PollEvents();

  if (inputHandler.IsKeyPressed(GLFW_KEY_ESCAPE)) {
    window.Close();
  }

  // Camera keyboard
  glm::vec3 cameraDirection = glm::vec3(0.0f);
  if (inputHandler.IsKeyDown(GLFW_KEY_W))
    cameraDirection += camera->GetFront();
  if (inputHandler.IsKeyDown(GLFW_KEY_S))
    cameraDirection -= camera->GetFront();
  if (inputHandler.IsKeyDown(GLFW_KEY_D))
    cameraDirection += camera->GetRight();
  if (inputHandler.IsKeyDown(GLFW_KEY_A))
    cameraDirection -= camera->GetRight();

  if (inputHandler.IsKeyDown(GLFW_KEY_LEFT_SHIFT))
    cameraDirection -= glm::vec3(0.0f, 1.0f, 0.0f);
  
  if (inputHandler.IsKeyDown(GLFW_KEY_SPACE))
    cameraDirection += glm::vec3(0.0f, 1.0f, 0.0f);

  camera->SetMoveDirection(cameraDirection);

  // Camera mouse

  float yaw = camera->GetYaw();
  yaw += inputHandler.GetMouseDeltaX();

  float pitch = camera->GetPitch();
  pitch -= inputHandler.GetMouseDeltaY();
  pitch = std::clamp(pitch, -89.0f, 89.0f);

  camera->UpdateViewDirection(yaw, pitch);
}

void Game::Update() {
  PROFILE_FUNCTION(Game)
  camera->Move();

  glm::vec3 pos = camera->GetPosition();

  ui.SetPlayerPosition(pos);
  ui.SetChunkPosition(world.GetChunkPosFromCoords(pos));
  ui.SetClimateValues(world.GetTemperature(pos.x, pos.z), world.GetHumidity(pos.x, pos.z), world.GetBiome(pos.x, pos.z));
}

void Game::Render(double) {
  PROFILE_FUNCTION(Game)

  world.Update(camera->GetPosition());
  ui.Arrange();

  renderer.ClearBackground(glm::vec3(0.0f));


  renderer.Begin3D(camera);

    if (inputHandler.IsKeyPressed(GLFW_KEY_TAB)) {
      renderer.ToggleWireframeMode();
    }

#ifdef GFX_ShadowMapping
    renderer.RenderShadows(world);
#endif

    renderer.RenderWorld(world);

  renderer.End3D();
  
  renderer.RenderUI(ui);

  window.SwapBuffers();
}

}
