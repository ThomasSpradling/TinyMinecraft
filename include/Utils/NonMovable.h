#ifndef NONMOVEABLE_H_
#define NONMOVEABLE_H_

namespace TinyMinecraft {

  namespace Utils {

    class NonMoveable {
    protected:
      NonMoveable() = default;
      ~NonMoveable() = default;

      NonMoveable(NonMoveable &&other) = delete;
      auto operator=(NonMoveable &&other) -> NonMoveable & = delete;

    };

  }

}

#endif // NONMOVEABLE_H_
