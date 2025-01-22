#include "Utils/Profiler.h"
#include "Utils/Logger.h"
#include <chrono>
#include <sstream>
#include <string>

namespace Utils {
  
std::unordered_map<ProfileCategory, std::unordered_map<std::string, std::vector<long long>>> Profiler::timings;

Profiler::~Profiler() {
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  Profiler::LogDuration(category, name, duration);
}

void Profiler::LogSummary() {
  std::ostringstream oss;

  oss << "\n\n------ Profiling Summary ------\n";

  for (const auto &[cat, sections] : timings) {
    oss << "\n\t" << CategoryToString(cat) << "\n";

    oss << std::string(60, '-') << "\n";

    for (const auto& [section, durations] : sections) {
      float total = 0;
      for (const auto& duration : durations) {
        total += duration;
      }
      int average = total / durations.size();
      oss << std::setw(40) << std::left << section
          << " | Runs: " << std::setw(9) << durations.size()
          << " | Avg: " << std::setw(9) << FormatDuration(average)
          << " | Total: " << FormatDuration(total) << "\n";
    }
  }

  g_logger.Log(Logger::Level::Profile, oss.str());
}

std::string Profiler::FormatDuration(long long nanoseconds) {
  std::ostringstream oss;

  if (nanoseconds < 1e4) {
    oss << nanoseconds << " ns";
  } else if (long long microseconds = nanoseconds / 1e3; microseconds < 1e4) {
    oss << microseconds << " µs";
  } else if (long milliseconds = microseconds / 1e3; milliseconds < 1e4) {
    oss << milliseconds << " ms";
  } else {
    int seconds = milliseconds / 1e3;
    oss << seconds << " sec";
  }

  return oss.str();
}

void Profiler::LogDuration(ProfileCategory cat, const std::string &section, long long duration) {
  timings[cat][section].push_back(duration);

#ifdef UTILS_ProfileVerbose
  g_logger.Log(Logger::Level::Profile, "({}) Time: {}", section, FormatDuration(duration));
#endif
}

std::string Profiler::CategoryToString(ProfileCategory cat) { 
  switch (cat) {
    case ProfileCategory::Window:
      return "Window";
    case ProfileCategory::UserInterface:
      return "UserInterface";
    case ProfileCategory::World:
      return "World";
    case ProfileCategory::Chunk:
      return "Chunk";
    case ProfileCategory::Game:
      return "Game";
    case ProfileCategory::Graphics:
      return "Graphics";
    case ProfileCategory::Miscellaneous:
    default:
      return "Miscellaneous";
  }
}

}
