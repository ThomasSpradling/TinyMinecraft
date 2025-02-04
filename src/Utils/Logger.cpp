#include "Utils/Logger.h"

namespace TinyMinecraft {

  namespace Utils {

    Logger const g_logger;

    void Logger::SetOutputCallback(std::function<void(const std::string &)> func) {
      outputCallback = std::move(func);
    }

    auto Logger::GetColor(Level level) const -> std::string {
      switch (level) {
        case Level::Debug: return BLUE;
        case Level::Warning: return YELLOW;
        case Level::Profile: return MAGENTA;
        case Level::Error:
        case Level::Fatal: return RED;
        default: return RESET;
      }
    }

    auto Logger::LevelToString(Level level) const -> std::string {
      switch (level) {
        case Level::Message: return "MESSAGE";
        case Level::Debug: return "DEBUG";
        case Level::Profile: return "PROFILE";
        case Level::Warning: return "WARNING";
        case Level::Error: return "ERROR";
        case Level::Fatal: return "FATAL";
        default: return "UNKNOWN";
      }
    }

  }

}
