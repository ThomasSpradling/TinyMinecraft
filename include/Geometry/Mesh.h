#ifndef MESH_H_
#define MESH_H_

#include "Graphics/BufferObject.h"
#include "Graphics/VertexArray.h"
#include "Utils/NonCopyable.h"
#include "Utils/mathgl.h"

namespace TinyMinecraft {

  namespace Geometry {

    struct MeshVertex {
      glm::vec3 position;
      glm::vec4 color;
      glm::vec2 texCoords;
      glm::vec3 normal;
    };

    class Mesh : private Utils::NonCopyable {
    public:
      Mesh();

      void ClearBuffers() {
        if (m_vertexCount > 0) {
          m_vao.Bind();
          m_vbo.CleanBuffers();
          m_ebo.CleanBuffers();
          m_vertexCount = 0;
        }
      }

      void Update(std::vector<MeshVertex> &vertices, std::vector<GLuint> &indices);
      [[nodiscard]] inline auto GetVertexCount() const -> size_t { return m_vertexCount; }
      inline void BindVertexArray() { m_vao.Bind(); }

    private:
      Graphics::VertexArray m_vao;
      Graphics::BufferObject m_vbo, m_ebo;

      size_t m_vertexCount = 0;
    };

  }

}

#endif // MESH_H_
