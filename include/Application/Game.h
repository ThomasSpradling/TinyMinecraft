#ifndef GAME_H_
#define GAME_H_

#include "Scene/Camera.h"
#include "Graphics/Renderer.h"
#include "Application/Window.h"
#include "World/World.h"

namespace Application {

class Game {
public:
  Game();

  /* Runs the Update() and Render() functions via a fixed-timestep game loop. */
  void Run();

private:
  static constexpr int viewportWidth = 1920;
  static constexpr int viewportHeight = 1080;

  Window m_window;
  InputHandler m_inputHandler;
  Graphics::Renderer m_renderer;
  World::World m_world;
  UI::UserInterface m_ui;
  
  std::shared_ptr<Scene::Camera> m_camera;

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