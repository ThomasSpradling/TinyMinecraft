#ifndef PROFILER
#define PROFILER

#include "Utils/NonCopyable.h"
#include "Utils/NonMovable.h"
#include "Utils/Singleton.h"
#include <chrono>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef __APPLE__
  #include <mach/mach.h>
#elif defined(__linux__)
  #include <sys/resource.h>
#endif

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

    struct ProfileData {
      long long duration;    // nanoseconds
      long long memoryDiff;  // bytes
    };

    class Profiler : private NonCopyable, private NonMoveable {
    public:
      Profiler(std::string_view section, ProfileCategory category = ProfileCategory::Miscellaneous);
      ~Profiler();

      static void LogSummary();

    private:
      ProfileCategory m_category;
      std::string m_name;
      std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
      long long m_memoryStart;

      static std::unordered_map<ProfileCategory, std::unordered_map<std::string, std::vector<ProfileData>>> records;
      static std::mutex recordMutex;

      static auto GetCurrentMemoryUsage() -> size_t;

      void LogRecord(ProfileCategory cat, const std::string &section, long long duration, long long memDiff);

      static auto CategoryToString(ProfileCategory category) -> std::string;

      static auto FormatDuration(long long nanoseconds) -> std::string;
      static auto FormatMemory(long long bytes) -> std::string;

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
  #define PROFILE_FUNCTION_Misc() do{} while(0);
  #define PROFILE_SCOPE_Misc(name) do{} while(0);
  #define PROFILE_FUNCTION(category) do{} while(0);
  #define PROFILE_SCOPE(category, name) do{} while(0);
#endif

#endif // PROFILER
