#include "Application/Game.h"
#include "Utils/Profiler.h"

auto main() -> int {
  Application::Game game;
  game.Run();

#ifdef UTILS_RunProfile
  Utils::Profiler::LogSummary();
#endif

  return 0;
}
