#ifndef LOGGER_H_
#define LOGGER_H_

#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include "mathgl.h"

namespace Utils {

class Logger {
public:
  enum class Level {
    Message,
    Debug,
    Profile,
    Warning,
    Error,
    Fatal
  };

  Logger() = default;

  void SetOutputCallback(std::function<void(const std::string &)> func);

  template<typename... Args>
  void Log(Level level, const std::string &format, Args... args) {
    std::ostringstream oss;

    if constexpr (sizeof...(args)  > 0) FormatString(oss, format, args...);
    else oss << format;

    std::string color = GetColor(level);
    std::string levelStr = LevelToString(level);

    std::ostringstream out;
    out << color << "[ " << std::left << std::setw(8) << levelStr << "] " << oss.str() << RESET << "\n";

    outputCallback(out.str());
  }

  template<typename... Args>
  void Message(const std::string &format, Args... args) {
    Log(Level::Message, format, args...);
  }
  
  template<typename... Args>
  void Debug(const std::string &format, Args... args) {
    Log(Level::Debug, format, args...);
  }
  
  template<typename... Args>
  void Warning(const std::string &format, Args... args) {
    Log(Level::Warning, format, args...);
  }

  template<typename... Args>
  void Error(const std::string &format, Args... args) {
    Log(Level::Error, format, args...);
  }

  template<typename... Args>
  void Fatal(const std::string &format, Args... args) {
    Log(Level::Fatal, format, args...);
  }

private:
  const std::string RESET = "\033[0m";
  const std::string YELLOW = "\033[33m";
  const std::string RED = "\033[31m";
  const std::string MAGENTA = "\033[35m";
  const std::string BLUE = "\033[34m";

  std::function<void(const std::string &)> outputCallback = [](const std::string &msg) {
    std::cout << msg;
  };

  std::string GetColor(Level level) const;

  std::string LevelToString(Level level) const;

  template<typename T>
  void FormatString(std::ostringstream &oss, const std::string &format, const T &value) {
    size_t placeholderPos = format.find("{}");
    if (placeholderPos != std::string::npos) {
      oss << format.substr(0, placeholderPos) << ToString(value) << format.substr(placeholderPos + 2);
    } else {
      oss << format;
    }
  }

  template<typename T, typename... Args>
  void FormatString(std::ostringstream &oss, const std::string& format, const T& value, Args... args) {
    size_t placeholderPos = format.find("{}");
    if (placeholderPos != std::string::npos) {
        oss << format.substr(0, placeholderPos) << ToString(value);
        FormatString(oss, format.substr(placeholderPos + 2), args...);
    } else {
        oss << format;
    }
  }

  template<typename T>
  std::string ToString(const T &value) {
    if constexpr (std::is_arithmetic_v<T>) {
      return std::to_string(value);
    } else {
      std::ostringstream oss;
      oss << value;
      return oss.str();
    }
  }

  std::string ToString(const glm::vec3 &vec) {
    std::ostringstream oss;
    oss << "vec3(" << vec.x << ", " << vec.y <<  ", " << vec.z << ")";
    return oss.str();
  }
};

extern Logger g_logger;

}

#endif // LOGGER_H_
