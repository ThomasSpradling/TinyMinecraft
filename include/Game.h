#ifndef GAME_H_
#define GAME_H_

#include "Camera.h"
#include "Renderer.h"
#include "Window.h"

class Game {
public:
  Game();
  ~Game();

  /* Sets the maximum frames per second (FPS) we hope to achieve. */
  void SetTargetFPS(int fps);

  /* Runs the Update() and Render() functions via a fixed-timestep game loop. */
  void EventLoop();

  /* Initializes all main game logic. */
  void Init();

  /* Handles all game inputs. */
  void ProcessInput();

private:
  int targetFPS = 60;
  double updateInterval;

  Window window;
  Renderer renderer;
  
  std::shared_ptr<Camera> camera;

  /* Game updating logic. */
  void Update();

  /* Game rendering logic where `0 <= interpolation <= 1` is the fraction of a frame we intend to try
    to render ahead of the current frame. */
  void Render(double interpolation);
};

#endif // GAME_H_