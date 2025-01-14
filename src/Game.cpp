#include "Game.h"
#include "GLFW/glfw3.h"
#include "gfx.h"
#include <MacTypes.h>
#include <cstdarg>

Game::Game() : window() {

}

Game::~Game() {

}

void Game::SetTargetFPS(int fps) {
  targetFPS = fps;
}

void Game::Init() {
  window.Init(800, 450, "Basic Window!");
  
  SetTargetFPS(60);
}

void Game::EventLoop() {
  double lastUpdateTime, lastRenderTime, currentTime;

  while (!window.ShouldClose()) {

    // events
    glfwPollEvents();

    if (window.IsKeyPressed(GLFW_KEY_ESCAPE)) {
      window.Close();
    }

    if (window.IsKeyDown(GLFW_KEY_A)) {
      std::cout << "A" << std::endl;
    }

    if (window.IsKeyPressed(GLFW_KEY_E)) {
      std::cout << "E" << std::endl;
    }

    currentTime = glfwGetTime();
    double updateDeltaTime = currentTime - lastUpdateTime;
    Update(updateDeltaTime);

    // Throttle to a specfic FPS
    double renderDeltaTime = currentTime - lastRenderTime;
    
    if (renderDeltaTime >= 1.0f / targetFPS) {
      Render(renderDeltaTime);
      lastRenderTime = currentTime;
    }

    lastUpdateTime = currentTime;
  }

  glfwTerminate();
}

//// Game Logic

void Game::Update(double) {
}

void Game::Render(double) {
  window.StartDrawing();

  glClear(GL_COLOR_BUFFER_BIT);
  // Stuff

  window.StopDrawing();
}