#ifndef VERTEX_ARRAY_H_
#define VERTEX_ARRAY_H_

#include "Graphics/gfx.h"

namespace Graphics {

class VertexArray {
public:
  VertexArray();

  ~VertexArray();
  VertexArray(const VertexArray &) = delete;
  auto operator=(const VertexArray &) -> VertexArray & = delete;
  VertexArray(VertexArray &&other) noexcept;
  auto operator=(VertexArray &&other) noexcept -> VertexArray &;

  void AddAttribute(GLuint id, int attributeSize, GLenum attributeType, int stride, int offset);
  void Bind();
private:
  GLuint m_handle;
};

}

#endif // VERTEX_ARRAY_H_
