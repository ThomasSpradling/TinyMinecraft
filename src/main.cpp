#include "Game.h"
#include <iostream>

int main(void) {
  Game game {};

  game.Init();
  game.EventLoop();

  return 0;
}
