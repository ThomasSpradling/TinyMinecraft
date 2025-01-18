#include "Application/Game.h"
#include <iostream>

int main(void) {
  Application::Game game;

  game.Initialize();
  game.Run();

  game.Shutdown();

  return 0;
}
