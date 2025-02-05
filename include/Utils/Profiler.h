#ifndef SCOPED_TIMER_
#define SCOPED_TIMER_

#include <chrono>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include "defs.h"

namespace TinyMinecraft {

  namespace Utils {

    enum class ProfileCategory : uint8_t {
      Miscellaneous = 0,
      Window,
      UserInterface,
      World,
      Chunk,
      Graphics,
      Game,
    };

    class Profiler {
    public:
      Profiler(std::string_view section, ProfileCategory category = ProfileCategory::Miscellaneous);
      
      ~Profiler();
      Profiler(const Profiler &) = delete;
      auto operator=(const Profiler &) -> Profiler & = delete;
      Profiler(Profiler &&) = delete;
      auto operator=(Profiler &&) -> Profiler & = delete;

      static void Start(const std::string &section, ProfileCategory cat = ProfileCategory::Miscellaneous);
      static void End();

      static void LogSummary();

    private:
      ProfileCategory m_category;
      std::string m_name;
      std::chrono::time_point<std::chrono::high_resolution_clock> m_start;

      static std::unordered_map<ProfileCategory, std::unordered_map<std::string, std::vector<long long>>> timings;
      static std::mutex timingMutex;

      static auto CategoryToString(ProfileCategory category) -> std::string;

      static auto FormatDuration(long long nanoseconds) -> std::string;

      void LogDuration(ProfileCategory cat, const std::string &section, long long duration);


    };

  }

}


// TODO: fix link
// hack: https://stackoverflow.com/questions/35922268/create-an-identifier-at-compile-time-that-has-the-line-number-as-part-of-it
#define CAT_(a, b) a ## b
#define CAT(a, b) CAT_(a, b)
#define VARNAME(Var) CAT(Var, __LINE__)

#ifdef UTILS_RunProfile
  #define PROFILE_FUNCTION_Misc() TinyMinecraft::Utils::Profiler VARNAME(timer) {__FUNCTION__};
  #define PROFILE_SCOPE_Misc(name) TinyMinecraft::Utils::Profiler VARNAME(timer) {name};
  #define PROFILE_FUNCTION(category) TinyMinecraft::Utils::Profiler VARNAME(timer) {__FUNCTION__, Utils::ProfileCategory::category};
  #define PROFILE_SCOPE(category, name) TinyMinecraft::Utils::Profiler VARNAME(timer) {name, Utils::ProfileCategory::category};
#else
  #define PROFILE_FUNCTION() do {} while (0);
  #define PROFILE_SCOPE(name) do {} while (0);
#endif

#endif // SCOPED_TIMER_
