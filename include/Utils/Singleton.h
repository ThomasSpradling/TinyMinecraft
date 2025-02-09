#ifndef SINGLETON_H_
#define SINGLETON_H_

#include "Utils/NonCopyable.h"
#include "Utils/NonMovable.h"

namespace TinyMinecraft {

  namespace Utils {

    class Singleton : private NonCopyable, private NonMoveable {

    protected:
      Singleton() = default;
      virtual ~Singleton() = default;
    };

  }

}

#endif // SINGLETON_H_
