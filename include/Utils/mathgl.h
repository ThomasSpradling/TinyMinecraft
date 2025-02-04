#ifndef MATH_GL_H_
#define MATH_GL_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace TinyMinecraft {

  namespace Utils {

    struct IVec2Hash {
      auto operator()(const glm::ivec2 &vec) const noexcept -> std::size_t {
        std::size_t h1 = std::hash<int>()(vec.x);
        std::size_t h2 = std::hash<int>()(vec.y);
        return h1 ^ (h2 << 1);
      }
    };

  }

}

#endif // MATH_GL_H_
