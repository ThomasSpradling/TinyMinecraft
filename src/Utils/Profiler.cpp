#include "Utils/Profiler.h"
#include "Utils/Logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

namespace TinyMinecraft {

  namespace Utils {
    
    std::unordered_map<ProfileCategory, std::unordered_map<std::string, std::vector<ProfileData>>> Profiler::records;
    std::mutex Profiler::recordMutex;

    Profiler::Profiler(std::string_view section, ProfileCategory category)
      : m_category(category)
      , m_name(section)
      , m_start(std::chrono::high_resolution_clock::now())
      , m_memoryStart(static_cast<long long>(GetCurrentMemoryUsage()))
    {}

    Profiler::~Profiler() {
      auto end = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - m_start).count();
      long long memoryEnd = static_cast<long long>(GetCurrentMemoryUsage());
      long long memDiff = memoryEnd - m_memoryStart;

      Profiler::LogRecord(m_category, m_name, duration, memDiff);
    }

    auto Profiler::GetCurrentMemoryUsage() -> size_t {
#ifdef __APPLE__
      mach_task_basic_info info;
      mach_msg_type_number_t size = MACH_TASK_BASIC_INFO_COUNT;
      if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &size) == KERN_SUCCESS) {
        return info.resident_size;
      }
      return 0;
#elif define (__linux__)
      struct rusage usage;
      getrusage(RUSAGE_SELF, &usage);
      return usage.ru_maxrss * 1024;
#endif
    }

    void Profiler::LogSummary() { 
      std::lock_guard<std::mutex> lock(recordMutex);
      std::ostringstream oss;
      oss << "\n\n------ Profiling Summary ------\n";
      for (const auto &[cat, sections] : records) {
        oss << "\n\t" << CategoryToString(cat) << "\n";
        oss << std::string(80, '-') << "\n";
        for (const auto& [section, dataVec] : sections) {
          long long totalTime = 0;
          long long totalMemDiff = 0;
          for (const auto &data : dataVec) {
            totalTime += data.duration;
            totalMemDiff += data.memoryDiff;
          }
          long long avgTime = totalTime / static_cast<long long>(dataVec.size());
          long long avgMemDiff = totalMemDiff / static_cast<long long>(dataVec.size());
          oss << std::setw(40) << std::left << section
              << " | Runs: " << std::setw(9) << dataVec.size()
              << " | Avg Time: " << std::setw(12) << FormatDuration(avgTime)
              << " | Total Time: " << FormatDuration(totalTime)
              << " | Avg Δ Mem: " << std::setw(12) << FormatMemory(avgMemDiff)
              << " | Total Δ Mem: " << FormatMemory(totalMemDiff)
              << "\n";
        }
      }
      Logger::Log(Logger::Level::Profile, oss.str());
    }

    auto Profiler::FormatDuration(long long nanoseconds) -> std::string {
      std::ostringstream oss;
      if (nanoseconds < 10000) {
        oss << nanoseconds << " ns";
      } else if (long long microseconds = nanoseconds / 1000; microseconds < 10000) {
        oss << microseconds << " µs";
      } else if (long long milliseconds = microseconds / 1000; milliseconds < 10000) {
        oss << milliseconds << " ms";
      } else {
        long seconds = milliseconds / 1000;
        oss << seconds << " sec";
      }
      return oss.str();
    }

    auto Profiler::FormatMemory(long long bytes) -> std::string {
      std::ostringstream oss;
      if (std::abs(bytes) < 1024) {
        oss << bytes << " B";
      } else if (std::abs(bytes) < 1024 * 1024) {
        oss << (bytes / 1024.0) << " KB";
      } else if (std::abs(bytes) < 1024LL * 1024LL * 1024LL) {
        oss << (bytes / (1024.0 * 1024.0)) << " MB";
      } else {
        oss << (bytes / (1024.0 * 1024.0 * 1024.0)) << " GB";
      }
      return oss.str();
    }

    void Profiler::LogRecord(ProfileCategory cat, const std::string &section, long long duration, long long memDiff) {
      std::lock_guard<std::mutex> lock(recordMutex);
      records[cat][section].push_back({ duration, memDiff });

#ifdef UTILS_ProfileVerbose
      Logger::Log(Logger::Level::Profile, "({}) Time: {} -- Memory: {}", section, FormatDuration(duration), FormatMemory(memDiff));
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
