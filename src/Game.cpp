#include "Game.h"
#include "gfx.h"
#include <unistd.h>

Game::Game() : window(), renderer(window) {}
Game::~Game() {}

void Game::SetTargetFPS(int fps) {
  targetFPS = fps;
  updateInterval = 1.f / static_cast<float>(targetFPS);
}

void Game::EventLoop() {
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

//// Game Logic

void Game::ProcessInput() {
  glfwPollEvents();

  if (window.IsKeyPressed(GLFW_KEY_ESCAPE)) {
    window.Close();
  }
}

void Game::Init() {
  window.Init(800, 450, "Basic Window!");
  
  SetTargetFPS(60);

  renderer.Init();
}

void Game::Update() {

}

void Game::Render(double) {

  renderer.StartDrawing();

  renderer.ClearBackground(glm::vec3(0.1f));

  float t = glfwGetTime();
  renderer.DrawRectangle(0, 0, window.GetWidth(), window.GetHeight(), glm::vec3(0.0f, 1.f, 0.f));
  renderer.DrawRectangle(t * 50, 50, 50, 50, glm::vec3(1.0f, 0.f, 0.f));

  renderer.StopDrawing();
}