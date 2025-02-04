#ifndef BUFFER_OBJECT_H_
#define BUFFER_OBJECT_H_

#include "Graphics/gfx.h"
#include <array>
#include <vector>

namespace TinyMinecraft {

  namespace Graphics {

    class BufferObject {
    public:
      BufferObject(GLenum target);

      ~BufferObject();
      BufferObject(const BufferObject &) = delete;
      auto operator=(const BufferObject &) -> BufferObject & = delete;
      BufferObject(BufferObject &&other) noexcept ;
      auto operator=(BufferObject &&other) noexcept -> BufferObject & ;

      void Bind() const;
      
      template <typename T> void BufferData(const std::vector<T> &data, GLenum usage) const {
        Bind();
        glBufferData(m_target, static_cast<GLsizeiptr>(sizeof(T) * data.size()), data.data(), usage);
      }

      template <typename T, size_t Count> void BufferData(const std::array<T, Count> &data, GLenum usage) const {
        Bind();
        glBufferData(m_target, static_cast<GLsizeiptr>(sizeof(T) * Count), data.data(), usage);
      }
    private:
      GLuint m_handle;
      GLenum m_target;
    };

  }

}

#endif // BUFFER_OBJECT_H_
