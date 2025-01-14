#include "Game.h"
#include "GLFW/glfw3.h"
#include "gfx.h"
#include <MacTypes.h>
#include <cstdarg>
#include <unistd.h>

Game::Game() : window() {
}

Game::~Game() {

}

void Game::SetTargetFPS(int fps) {
  targetFPS = fps;
}

void Game::SetTargetGameHz(int updatesPerSecond) {
  targetGameHz = updatesPerSecond;
  updateInterval = 1.f / static_cast<float>(targetGameHz);
}

void Game::Init() {
  window.Init(800, 450, "Basic Window!");
  
  SetTargetGameHz(30);
  SetTargetFPS(100);
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

    // If still behind, try to catch up
    while (lag >= updateInterval || iters < maxCatchUpIterations) {
      Update();

      lag -= updateInterval;
      iters++;
    }

    Render(lag / updateInterval);
    lastRenderTime = glfwGetTime();

    while (currentTime - lastRenderTime < frameInterval && currentTime - lastRenderTime < updateInterval) {
      usleep(1000); // sleep for 1 ms

      currentTime = glfwGetTime();
    }
  }

  glfwTerminate();
}

void Game::ProcessInput() {
  glfwPollEvents();

  if (window.IsKeyPressed(GLFW_KEY_ESCAPE)) {
    window.Close();
  }
}

//// Game Logic

void Game::Update() {

}

void Game::Render(double) {
  window.StartDrawing();

  glClear(GL_COLOR_BUFFER_BIT);
  // Stuff

  window.StopDrawing();
}