#include "Graphics/WireframeRenderer.h"
#include "Graphics/BufferObject.h"
#include "Graphics/Shader.h"
#include "Graphics/VertexArray.h"
#include "Graphics/gfx.h"
#include "Scene/PlayerCameras.h"
#include "Utils/Logger.h"
#include "Utils/defs.h"
#include "glm/fwd.hpp"
#include <memory>

namespace TinyMinecraft {

  namespace Graphics {

    struct WireframeVertex {
      glm::vec3 position;
      glm::vec4 color;
    };

    static struct WireframeRendererData {
      std::unique_ptr<VertexArray> vao;
      std::unique_ptr<BufferObject> vbo, ebo;
      std::unique_ptr<Shader> shader;

      std::vector<WireframeVertex> vertices;
      std::vector<GLuint> indices;

      int indexCount = 0;

      glm::mat4 viewProjection { 1.0f };
    } s_data;

    void WireframeRenderer::Initialize() {
      glEnable(GL_LINE_SMOOTH);

      s_data.vao = std::make_unique<VertexArray>();
      s_data.vbo = std::make_unique<BufferObject>(GL_ARRAY_BUFFER);
      s_data.ebo = std::make_unique<BufferObject>(GL_ELEMENT_ARRAY_BUFFER);

      s_data.vao->AddAttribute(0, 3, GL_FLOAT, sizeof(WireframeVertex), offsetof(WireframeVertex, position));
      s_data.vao->AddAttribute(1, 4, GL_FLOAT, sizeof(WireframeVertex), offsetof(WireframeVertex, color));

      s_data.shader = std::make_unique<Shader>("../resources/shaders/Wireframe.vs", "../resources/shaders/Wireframe.fs");
    }
    
    void WireframeRenderer::Render(const std::shared_ptr<Scene::PlayerCamera> &camera) {
      
      glEnable(GL_DEPTH_TEST);
      glDisable(GL_CULL_FACE);

      s_data.viewProjection = camera->GetViewProjection();
      s_data.shader->Uniform("u_ViewProjection", s_data.viewProjection);

      if (s_data.indexCount > 0) {
        s_data.vao->Bind();
        s_data.shader->Use();

        s_data.vbo->BufferData(s_data.vertices, GL_DYNAMIC_DRAW);
        s_data.ebo->BufferData(s_data.indices, GL_DYNAMIC_DRAW);

        glDrawElements(GL_LINES, s_data.indexCount, GL_UNSIGNED_INT, nullptr);
        s_data.vertices.clear();
        s_data.indices.clear();
      }

      s_data.indexCount = 0;

      glEnable(GL_CULL_FACE);
      glDisable(GL_DEPTH_TEST);
    }

    void WireframeRenderer::Draw3DRectangle(glm::vec3 start, glm::vec3 end, glm::vec4 color) {
      GLuint vertexOffset = static_cast<GLuint>(s_data.vertices.size());

      s_data.vertices.push_back({ glm::vec4(start.x, start.y, start.z, 1.0f), color });
      s_data.vertices.push_back({ glm::vec4(end.x,   start.y, start.z, 1.0f), color });
      s_data.vertices.push_back({ glm::vec4(end.x,   end.y,   start.z, 1.0f), color });
      s_data.vertices.push_back({ glm::vec4(start.x, end.y,   start.z, 1.0f), color });
      s_data.vertices.push_back({ glm::vec4(start.x, start.y, end.z,   1.0f), color });
      s_data.vertices.push_back({ glm::vec4(end.x,   start.y, end.z,   1.0f), color });
      s_data.vertices.push_back({ glm::vec4(end.x,   end.y,   end.z,   1.0f), color });
      s_data.vertices.push_back({ glm::vec4(start.x, end.y,   end.z,   1.0f), color });

      s_data.indices.push_back(vertexOffset + 0); s_data.indices.push_back(vertexOffset + 1);
      s_data.indices.push_back(vertexOffset + 1); s_data.indices.push_back(vertexOffset + 2);
      s_data.indices.push_back(vertexOffset + 2); s_data.indices.push_back(vertexOffset + 3);
      s_data.indices.push_back(vertexOffset + 3); s_data.indices.push_back(vertexOffset + 0);

      s_data.indices.push_back(vertexOffset + 4); s_data.indices.push_back(vertexOffset + 5);
      s_data.indices.push_back(vertexOffset + 5); s_data.indices.push_back(vertexOffset + 6);
      s_data.indices.push_back(vertexOffset + 6); s_data.indices.push_back(vertexOffset + 7);
      s_data.indices.push_back(vertexOffset + 7); s_data.indices.push_back(vertexOffset + 4);

      s_data.indices.push_back(vertexOffset + 0); s_data.indices.push_back(vertexOffset + 4);
      s_data.indices.push_back(vertexOffset + 1); s_data.indices.push_back(vertexOffset + 5);
      s_data.indices.push_back(vertexOffset + 2); s_data.indices.push_back(vertexOffset + 6);
      s_data.indices.push_back(vertexOffset + 3); s_data.indices.push_back(vertexOffset + 7);

      s_data.indexCount += 24;
    }

    void WireframeRenderer::DrawBlock(glm::vec3 pos, glm::vec4 color) {
      constexpr float epsilon = 1e-3;
      Draw3DRectangle(pos, pos + glm::vec3(1.0f), color);
    }

    void WireframeRenderer::DrawChunk(glm::ivec2 chunkPos, glm::vec4 color) {
      const glm::vec3 start = glm::vec3(CHUNK_WIDTH * chunkPos.x, 0.0f, CHUNK_LENGTH * chunkPos.y);
      const glm::vec3 end = start + glm::vec3(CHUNK_WIDTH, 16.0f, CHUNK_LENGTH);

      glm::vec3 pos = glm::vec3(CHUNK_WIDTH * chunkPos.x, 0, CHUNK_LENGTH * chunkPos.y);
      for (int i = 0; i < CHUNK_HEIGHT / 16; ++i) {
        glm::vec3 offset = glm::vec3(0.0f, i * 16, 0.0f);
        Draw3DRectangle(start + offset, end + offset, color);
      }
    }

  }

}
