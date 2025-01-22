#ifndef GAME_H_
#define GAME_H_

#include "Scene/Camera.h"
#include "Graphics/Renderer.h"
#include "Application/Window.h"
#include "World/World.h"

namespace Application {

class Game {
public:
  Game() = default;

  /* Initializes all main game logic. */
  void Initialize();

  /* Runs the Update() and Render() functions via a fixed-timestep game loop. */
  void Run();

  void Shutdown();

private:
  Window window;
  InputHandler inputHandler;
  Graphics::Renderer renderer;
  World::World world;
  UI::UserInterface ui;
  
  std::shared_ptr<Scene::Camera> camera;

  /* Handles all game inputs. */
  void ProcessInput();

  /* Game updating logic. */
  void Update();

  /* Game rendering logic where `0 <= interpolation <= 1` is the fraction of a frame we intend to try
    to render ahead of the current frame. */
  void Render(double interpolation);
};

}

#endif // GAME_H_