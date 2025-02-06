#include "Geometry/Mesh.h"

namespace TinyMinecraft {

  namespace Geometry {

    Mesh::Mesh()
      : m_vbo(GL_ARRAY_BUFFER)
      , m_ebo(GL_ELEMENT_ARRAY_BUFFER)
    {
      m_vao.Bind();
      m_vbo.Bind();
      m_ebo.Bind();

      m_vao.AddAttribute(0, 3, GL_FLOAT, sizeof(MeshVertex), offsetof(MeshVertex, position));
      m_vao.AddAttribute(1, 4, GL_FLOAT, sizeof(MeshVertex), offsetof(MeshVertex, color));
      m_vao.AddAttribute(2, 2, GL_FLOAT, sizeof(MeshVertex), offsetof(MeshVertex, texCoords));
      m_vao.AddAttribute(3, 3, GL_FLOAT, sizeof(MeshVertex), offsetof(MeshVertex, normal));
    }

    // Mesh::Mesh(Mesh &&other) noexcept
    // {}

    // auto Mesh::operator=(Mesh &&other) noexcept -> Mesh & {
    //   Mesh temp(std::move(other));
    //   std::swap(*this, temp);
    //   return *this;
    // }

    void Mesh::Update(std::vector<MeshVertex> &vertices, std::vector<GLuint> &indices) {
      if (vertices.size() == 0)
        return;

      m_vao.Bind();
      
      m_vbo.BufferData(vertices, GL_DYNAMIC_DRAW);
      m_ebo.BufferData(indices, GL_DYNAMIC_DRAW);

      m_vertexCount = indices.size();

      vertices.clear();
      vertices.shrink_to_fit();
      indices.clear();
      indices.shrink_to_fit();
    }

  }

}
