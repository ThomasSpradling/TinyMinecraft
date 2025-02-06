#include "Application/Game.h"
#include "Utils/Logger.h"
#include "Utils/Profiler.h"
#include "Utils/utils.h"
#include <unistd.h>

using namespace TinyMinecraft;

auto main() -> int {
  Utils::g_logger.Message("Press anything to start. PID: {}", getpid());
  std::cin.get();

  Utils::SetThreadName("main");

  Application::Game game;
  game.Run();

#ifdef UTILS_RunProfile
  Utils::Profiler::LogSummary();
#endif

  return 0;
}
