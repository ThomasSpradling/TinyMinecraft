#include "Utils/Logger.h"

namespace Utils {

Logger g_logger;

void Logger::SetOutputCallback(std::function<void(const std::string &)> func) {
  outputCallback = std::move(func);
}

std::string Logger::GetColor(Level level) const {
  switch (level) {
    case Level::Debug: return BLUE;
    case Level::Warning: return YELLOW;
    case Level::Profile: return MAGENTA;
    case Level::Error:
    case Level::Fatal: return RED;
    default: return RESET;
  }
}

std::string Logger::LevelToString(Level level) const {
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
