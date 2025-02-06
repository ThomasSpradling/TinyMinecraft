#include "Graphics/BufferObject.h"
#include <utility>

namespace TinyMinecraft {

  namespace Graphics {

    BufferObject::BufferObject(GLenum target)
      : m_target(target)
    {
      glGenBuffers(1, &m_handle);
      Bind();
    }

    BufferObject::~BufferObject() {
      glDeleteBuffers(1, &m_handle);
    }

    BufferObject::BufferObject(BufferObject &&other) noexcept
      : m_handle(std::exchange(other.m_handle, 0))
      , m_target(other.m_target)
    {}

    auto BufferObject::operator=(BufferObject &&other) noexcept -> BufferObject & {
      BufferObject temp(std::move(other));
      std::swap(m_handle, temp.m_handle);
      std::swap(m_target, temp.m_target);

      return *this;
    }

    void BufferObject::Bind() const {
      glBindBuffer(m_target, m_handle);
    }

    void BufferObject::Unbind() const {
      glBindBuffer(m_target, 0);
    }

  }

}
