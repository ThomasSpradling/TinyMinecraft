#include "Application/Game.h"
#include "Application/InputHandler.h"
#include "Graphics/gfx.h"
#include <unistd.h>

namespace Application {

void Game::Initialize() {
  SetTargetFPS(60);

  const int viewportWidth = 800;
  const int viewportHeight = 450;

  window.Init(viewportWidth, viewportHeight, "Look mum! I made a chunk!", inputHandler);
  renderer.Init(viewportWidth, viewportHeight);
  world.Generate();
  ui.Initialize();
  
  camera = std::make_shared<Scene::Camera>();

  camera->SetPosition(glm::vec3(50.f, 50.f, 50.f));
  camera->SetTarget(glm::vec3(0.f, 32.f, 0.f));
  camera->SetUpVector(glm::vec3(0.f, 1.f, 0.f));
  
  camera->SetFOV(45.f);
  camera->SetAspectRatio(static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight()));
}

void Game::Run() {
  const int maxCatchUpIterations = 10;

  double previousTime = glfwGetTime();
  double lastRenderTime = glfwGetTime();
  double lag = 0.0; // How far behind we are on updates

  double frameInterval = 1.f / static_cast<float>(targetFPS); // Seconds per frame

  while (!window.ShouldClose()) {
    double currentTime = glfwGetTime();
    double elapsedTime = currentTime - previousTime;
    previousTime = currentTime;
    lag += elapsedTime;

    ProcessInput();

#if _DEBUG_
    double currentFPS = 1.f / elapsedTime;
    std::cout << "Current FPS: " << currentFPS << std::endl;
#endif

    int iters = 0;

    // If still behind, try to catch up. We use maxCatchUpIterations not to stall.
    while (lag >= updateInterval || iters < maxCatchUpIterations) {
      Update();

      lag -= updateInterval;
      iters++;
    }

    Render(lag / updateInterval);

    // Ensure we've waited enough time to adhere to our maximal FPS
    lastRenderTime = glfwGetTime();
    while (currentTime - lastRenderTime < frameInterval && currentTime - lastRenderTime < updateInterval) {
      usleep(1000); // sleep for 1 ms
      currentTime = glfwGetTime();
    }
  }

  glfwTerminate();
}

void Game::Shutdown() {

}

void Game::SetTargetFPS(int fps) {
  targetFPS = fps;
  updateInterval = 1.f / static_cast<float>(targetFPS);
}


// Private Methods

void Game::ProcessInput() {
  window.PollEvents();

  if (inputHandler.IsKeyPressed(GLFW_KEY_ESCAPE)) {
    window.Close();
  }
}

void Game::Update() {
  float t = glfwGetTime() / 5.f;
  float x = 50.f * glm::cos(2 * glm::pi<float>() * t);
  float z = 50.f * glm::sin(2 * glm::pi<float>() * t);
  camera->SetPosition(glm::vec3(x, 60.f, z));
}

void Game::Render(double) {
  world.Update();
  ui.Arrange();

  renderer.ClearBackground(glm::vec3(0.1f));


  renderer.Begin3D(camera);

    if (inputHandler.IsKeyPressed(GLFW_KEY_F3)) {
      renderer.ToggleWireframeMode();
    }

    renderer.RenderWorld(world);

  renderer.End3D();
  
  renderer.RenderUI(ui);

  window.SwapBuffers();
}

}
