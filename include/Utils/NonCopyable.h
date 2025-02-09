#ifndef NONCOPYABLE_H_
#define NONCOPYABLE_H_

namespace TinyMinecraft {

  namespace Utils {

    class NonCopyable {
    protected:
      NonCopyable() = default;
      ~NonCopyable() = default;

      NonCopyable(const NonCopyable &other) = delete;
      auto operator=(const NonCopyable &other) -> NonCopyable & = delete;

    };

  }

}

#endif // NONCOPYABLE_H_
