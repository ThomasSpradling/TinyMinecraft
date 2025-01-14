#ifndef GAME_H_
#define GAME_H_

#include "Window.h"
class Game {
public:
  Game();
  ~Game();

  void SetTargetFPS(int fps);
  void SetTargetGameHz(int updatesPerSecond);

  void Init();
  void EventLoop();

  void ProcessInput();

private:
  int targetFPS = 60;
  int targetGameHz = 30;
  double updateInterval;

  Window window;

  // Updates at a fixed time-step `updateInterval`, based on the target FPS.
  void Update();

  // Renders up to this frame and the fraction of a frame given.
  void Render(double frameFraction);
};

#endif // GAME_H_