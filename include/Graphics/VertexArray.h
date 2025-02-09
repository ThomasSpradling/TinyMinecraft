#ifndef VERTEX_ARRAY_H_
#define VERTEX_ARRAY_H_

#include "Graphics/gfx.h"
#include "Utils/NonMovable.h"

namespace TinyMinecraft {

  namespace Graphics {

    class VertexArray : Utils::NonMoveable {
    public:
      VertexArray();
      ~VertexArray();
      
      VertexArray(VertexArray &&other) noexcept;
      auto operator=(VertexArray &&other) noexcept -> VertexArray &;

      void AddAttribute(GLuint id, int attributeSize, GLenum attributeType, int stride, int offset);
      inline void Bind() const { glBindVertexArray(m_handle); }
      inline void Unbind() const { glBindVertexArray(0); };
    private:
      GLuint m_handle;
    };

  }

}

#endif // VERTEX_ARRAY_H_
