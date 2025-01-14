#ifndef GAME_H_
#define GAME_H_

#include "Window.h"
class Game {
public:
  Game();
  ~Game();

  void SetTargetFPS(int fps);

  void Init();
  void EventLoop();

private:
  int targetFPS = 60;

  Window window;

  void Update(double dt);
  void Render(double dt);
};

#endif // GAME_H_