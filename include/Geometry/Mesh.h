#ifndef MESH_H_
#define MESH_H_

#include "Graphics/BufferObject.h"
#include "Graphics/VertexArray.h"
#include "Utils/mathgl.h"

namespace Geometry {

struct MeshVertex {
  glm::vec3 position;
  glm::vec4 color;
  glm::vec2 texCoords;
  glm::vec3 normal;
};

class Mesh {
public:
  Mesh();

  // ~Mesh() = default;
  // Mesh(const Mesh &) = delete;
  // auto operator=(const Mesh &) -> Mesh & = delete;
  // Mesh(Mesh &&other) noexcept;
  // auto operator=(Mesh &&other) noexcept -> Mesh &;

  void Update(std::vector<MeshVertex> &vertices, std::vector<GLuint> &indices);
  [[nodiscard]] auto GetVertexCount() const -> size_t { return m_vertexCount; }
  void BindVertexArray() { m_vao.Bind(); }

private:
  Graphics::VertexArray m_vao;
  Graphics::BufferObject m_vbo, m_ebo;

  size_t m_vertexCount = 0;
};

}

#endif // MESH_H_
