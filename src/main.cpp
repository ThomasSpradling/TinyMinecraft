#include "Game.h"
#include <iostream>

int main(void) {
  // std::cout << "Hello, world" << std::endl;

  Game game {};

  game.Init();
  game.EventLoop();

  return 0;
}
