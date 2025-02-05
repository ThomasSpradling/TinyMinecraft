#include "Utils/Profiler.h"
#include "Utils/Logger.h"
#include <chrono>
#include <sstream>
#include <string>

namespace TinyMinecraft {

  namespace Utils {
    
    std::unordered_map<ProfileCategory, std::unordered_map<std::string, std::vector<long long>>> Profiler::timings;
    std::mutex Profiler::timingMutex;

    Profiler::Profiler(std::string_view section, ProfileCategory category)
      : m_category(category)
      , m_name(section)
      , m_start(std::chrono::high_resolution_clock::now())
    {}

    Profiler::~Profiler() {
      auto end = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - m_start).count();

      Profiler::LogDuration(m_category, m_name, duration);
    }

    void Profiler::LogSummary() { 
      std::lock_guard<std::mutex> lock(timingMutex);
      std::ostringstream oss;

      oss << "\n\n------ Profiling Summary ------\n";

      for (const auto &[cat, sections] : timings) {
        oss << "\n\t" << CategoryToString(cat) << "\n";

        oss << std::string(60, '-') << "\n";

        for (const auto& [section, durations] : sections) {
          long long total = 0;
          for (const auto& duration : durations) {
            total += duration;
          }
          long long average = total / static_cast<long long>(durations.size());
          oss << std::setw(40) << std::left << section
              << " | Runs: " << std::setw(9) << durations.size()
              << " | Avg: " << std::setw(9) << FormatDuration(average)
              << " | Total: " << FormatDuration(total) << "\n";
        }
      }

      g_logger.Log(Logger::Level::Profile, oss.str());
    }

    auto Profiler::FormatDuration(long long nanoseconds) -> std::string {
      std::ostringstream oss;
      
      // auto nano = static_cast<double>(nanoseconds);

      if (nanoseconds < 10000) {
        oss << nanoseconds << " ns";
      } else if (long long microseconds = nanoseconds / 1000; microseconds < 10000) {
        oss << microseconds << " µs";
      } else if (long milliseconds = microseconds / 1000; milliseconds < 10000) {
        oss << milliseconds << " ms";
      } else {
        long seconds = milliseconds / 1000;
        oss << seconds << " sec";
      }

      return oss.str();
    }

    void Profiler::LogDuration(ProfileCategory cat, const std::string &section, long long duration) {
      std::lock_guard<std::mutex> lock(timingMutex);
      timings[cat][section].push_back(duration);

    #ifdef UTILS_ProfileVerbose
      g_logger.Log(Logger::Level::Profile, "({}) Time: {}", section, FormatDuration(duration));
    #endif
    }

    auto Profiler::CategoryToString(ProfileCategory cat) -> std::string { 
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

}
