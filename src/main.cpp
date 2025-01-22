#include "Application/Game.h"
#include "Utils/Profiler.h"

int main(void) {
  Application::Game game;
  game.Initialize();

  game.Run();

  game.Shutdown();

#ifdef UTILS_RunProfile
  Utils::Profiler::LogSummary();
#endif

  return 0;
}
