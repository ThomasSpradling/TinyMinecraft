#include "Application/Game.h"
#include "Utils/Profiler.h"
#include "Utils/utils.h"

auto main() -> int {
  Utils::SetThreadName("main");

  Application::Game game;
  game.Run();

#ifdef UTILS_RunProfile
  Utils::Profiler::LogSummary();
#endif

  return 0;
}
