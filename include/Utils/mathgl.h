#ifndef MATH_GL_H_
#define MATH_GL_H_

#include "glm/detail/qualifier.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ostream>

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
  
  template <typename T, glm::length_t L>
  auto operator<<(std::ostream &os, const glm::vec<L, T, glm::defaultp> &vec) noexcept -> std::ostream & {
    os << "(";
    for (glm::length_t i = 0; i < L; ++i) {
      os << vec[i];
      if (i < L - i) os << ", ";
    }
    os << ")";
    return os;
  }

  template <typename T, glm::length_t C, glm::length_t R>
  auto operator<<(std::ostream &os, const glm::mat<C, R, T, glm::defaultp> &mat) noexcept -> std::ostream & {
    os << "[\n";
    for (glm::length_t row = 0; row < R; ++row) {
      os << " ";
      for (glm::length_t col = 0; col < C; ++col) {
        os << mat[col][row];
        if (col < C - 1) os << ", ";
      }
      os << "\n";
    }
    os << "]";
    return os;
  }

}

#endif // MATH_GL_H_
