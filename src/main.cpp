#include "Application/Game.h"
#include "Utils/Logger.h"
#include "Utils/utils.h"
#include <iostream>
#include <unistd.h>

using namespace TinyMinecraft;

auto main() -> int {
  Utils::Logger::Message("Press anything to start. PID: {}", getpid());
  std::cin.get();

  Utils::SetThreadName("main");

  Application::Game game;
  game.Run();

#ifdef UTILS_RunProfile
  Utils::Profiler::LogSummary();
#endif

  return 0;
}
