#ifndef LOGGER_H_
#define LOGGER_H_

#include <cstdint>
#include <functional>
#include <sstream>
#include <string>
#include <type_traits>
#include "Utils/Printable.h"
#include "Utils/Singleton.h"
#include "Utils/utils.h"

namespace TinyMinecraft {

  namespace Utils {

    using LoggerFn = std::function<void(const std::string &)>;

    class Logger : private Singleton {
    public:
      enum class Level : uint8_t {
        Message,
        Debug,
        Profile,
        Warning,
        Error,
        Fatal
      };

      static void SetOutputCallback(LoggerFn func);

      template<typename... Args>
      static void Log(Level level, const std::string &format, Args... args) {
        std::ostringstream oss;

        if constexpr (sizeof...(args) > 0) FormatString(oss, format, args...);
        else oss << format;

        std::string color = GetColor(level);
        std::string levelStr = LevelToString(level);

        std::ostringstream out;
        out << color << "[ " << levelStr << " (" << Utils::GetThreadName() << ") ] " << oss.str() << RESET << "\n";

        GetOutputCallback()(out.str());
      }

      template<typename... Args>
      static void Message(const std::string &format, Args... args) {
        Log(Level::Message, format, args...);
      }

      template<typename... Args>
      static void Debug(const std::string &format, Args... args) {
        Log(Level::Debug, format, args...);
      }

      template<typename... Args>
      static void Warning(const std::string &format, Args... args) {
        Log(Level::Warning, format, args...);
      }

      template<typename... Args>
      static void Error(const std::string &format, Args... args) {
        Log(Level::Error, format, args...);
      }

      template<typename... Args>
      static void Fatal(const std::string &format, Args... args) {
        Log(Level::Fatal, format, args...);
      }

    private:
      static constexpr const char* RESET = "\033[0m";
      static constexpr const char* YELLOW = "\033[33m";
      static constexpr const char* RED = "\033[31m";
      static constexpr const char* MAGENTA = "\033[35m";
      static constexpr const char* BLUE = "\033[34m";

      static auto GetOutputCallback() -> LoggerFn &;
      static auto GetColor(Level level) -> std::string;
      static auto LevelToString(Level level) -> std::string;

      template<typename T>
      static void FormatString(std::ostringstream &oss, const std::string &format, const T &value) {
        size_t placeholderPos = format.find("{}");
        if (placeholderPos != std::string::npos) {
          oss << format.substr(0, placeholderPos) << ToString(value) << format.substr(placeholderPos + 2);
        } else {
          oss << format;
        }
      }

      template<typename T, typename... Args>
      static void FormatString(std::ostringstream &oss, const std::string& format, const T& value, Args... args) {
        size_t placeholderPos = format.find("{}");
        if (placeholderPos != std::string::npos) {
          oss << format.substr(0, placeholderPos) << ToString(value);
          FormatString(oss, format.substr(placeholderPos + 2), args...);
        } else {
          oss << format;
        }
      }

      template<typename T>
      static std::string ToString(const T &value) {
        if constexpr (std::is_arithmetic_v<T>) {
          return std::to_string(value);
        } else if constexpr (std::is_base_of_v<Printable, T>) {
          return value.ToString();
        } else {
          std::ostringstream oss;
          oss << value;
          return oss.str();
        }
      }
    };

  }

}

#endif // LOGGER_H_
