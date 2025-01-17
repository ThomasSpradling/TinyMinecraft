#include "Game.h"
#include "GLFW/glfw3.h"
#include "gfx.h"
#include "glm/fwd.hpp"
#include <memory>
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
  window.PollEvents();

  if (window.IsKeyPressed(GLFW_KEY_ESCAPE)) {
    window.Close();
  }
}

void Game::Init() {
  window.Init(800, 450, "Basic Window!");
  renderer.Init();
  
  SetTargetFPS(120);

  camera = std::make_shared<Camera>();

  camera->SetPosition(glm::vec3(50.f, 50.f, 50.f));
  camera->SetTarget(glm::vec3(0.f, 0.f, 0.f));
  camera->SetUpVector(glm::vec3(0.f, 1.f, 0.f));
  
  camera->SetFOV(45.f);
  camera->SetAspectRatio(static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight()));
  
}

void Game::Update() {
  float t = glfwGetTime() / 5.f;
  float x = 50.f * glm::cos(2 * glm::pi<float>() * t);
  float z = 50.f * glm::sin(2 * glm::pi<float>() * t);
  camera->SetPosition(glm::vec3(x, 50.f, z));
}

void Game::Render(double) {

  renderer.StartDrawing();
    renderer.ClearBackground(glm::vec3(0.1f));

    float t = glfwGetTime();

    renderer.Begin3D(camera);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      for (int i = 0; i <= 16; ++i) {
        for (int j = 0; j <= 16; ++j) {
          for (int k = 0; k <= 16; ++k) {
            float x = i * 2.f - 16.0f;
            float y = j * 2.f - 16.0f;
            float z = k * 2.f - 16.0f;
            renderer.DrawCube(glm::vec3(x, y, z), 2.f, glm::vec3(1.0f, 0.f, 0.f));
          }
        }
      }
      renderer.DrawCube(glm::vec3(0.0f, 0.f, 0.f), 2.f, glm::vec3(1.0f, 0.f, 0.f));

    renderer.End3D();

    renderer.DrawRectangle(t * 50, 50, 50, 50, glm::vec3(0.0f, 1.f, 0.f));

  renderer.StopDrawing();
}