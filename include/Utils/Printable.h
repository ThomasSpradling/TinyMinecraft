#ifndef PRINTABLE_H_
#define PRINTABLE_H_

#include <string>

namespace TinyMinecraft {

  namespace Utils {

    class Printable {
    public:
      virtual ~Printable() = default;
      [[nodiscard]] virtual auto ToString() const -> std::string = 0;

    };

  }

}

#endif // PRINTABLE_H_
