#include "Utils/Logger.h"
#include <iostream>

namespace TinyMinecraft {

  namespace Utils {

    void Logger::SetOutputCallback(LoggerFn func) {
      GetOutputCallback() = std::move(func);
    }

    auto Logger::GetOutputCallback() -> LoggerFn & {
      static std::function<void(const std::string &)> outputCallback = [](const std::string &msg) {
        std::cout << msg;
      };
      return outputCallback;
    }

    auto Logger::GetColor(Level level) -> std::string {
      switch (level) {
        case Level::Message: return RESET;
        case Level::Debug: return BLUE;
        case Level::Profile: return MAGENTA;
        case Level::Warning: return YELLOW;
        case Level::Error: return RED;
        case Level::Fatal: return RED;
        default: return RESET;
      }
    }

    auto Logger::LevelToString(Level level) -> std::string {
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
