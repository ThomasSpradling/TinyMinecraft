#include "Graphics/VertexArray.h"
#include "Utils/Logger.h"
#include <utility>

namespace TinyMinecraft {

  namespace Graphics {

    VertexArray::VertexArray() {
      glGenVertexArrays(1, &m_handle);
      Bind();
    }

    VertexArray::~VertexArray() {
      glDeleteVertexArrays(1, &m_handle);
    }

    VertexArray::VertexArray(VertexArray &&other) noexcept
      : m_handle(std::exchange(other.m_handle, 0))
    {}

    auto VertexArray::operator=(VertexArray &&other) noexcept -> VertexArray & {
      VertexArray temp(std::move(other));
      std::swap(m_handle, temp.m_handle);
      return *this;
    }

    void VertexArray::AddAttribute(GLuint id, int attributeSize, GLenum attributeType, int stride, int offset) {
      Bind();

      switch (attributeType) {
        case GL_HALF_FLOAT:
        case GL_FLOAT:
        case GL_DOUBLE:
          glVertexAttribPointer(id, attributeSize, attributeType, GL_FALSE, stride, reinterpret_cast<void *>(offset));
          break;
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_INT:
        case GL_UNSIGNED_INT:
          glVertexAttribIPointer(id, attributeSize, attributeType, stride, reinterpret_cast<void *>(offset));
          break;
        default:
          Utils::g_logger.Fatal("Graphics: VAO {} was assigned an invalid attribute.", m_handle);
          exit(1);
          break;
      }

      glEnableVertexAttribArray(id);
    }

    void VertexArray::Bind() {
      glBindVertexArray(m_handle);
    }

  }

}
