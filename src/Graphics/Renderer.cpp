#include "Graphics/Renderer.h"
#include "Graphics/Renderer2D.h"
#include "Graphics/WireframeRenderer.h"
#include "Scene/PlayerCameras.h"
#include "Utils/Profiler.h"
#include "World/Chunk.h"
#include "Graphics/gfx.h"
#include <algorithm>

namespace TinyMinecraft {

  namespace Graphics {

    Renderer::Renderer(float viewportWidth, float viewportHeight)
      : m_blockShader("../resources/shaders/block_solid.vs", "../resources/shaders/block_solid.fs")
      , m_waterShader("../resources/shaders/block_translucent.vs", "../resources/shaders/block_translucent.fs")
      , m_blockAtlasTexture("../resources/textures/block_atlas.png")
      , m_viewportWidth(viewportWidth)
      , m_viewportHeight(viewportHeight)
    {
      PROFILE_SCOPE(Graphics, "Renderer::Initialize")

      Renderer2D::Initialize();
      WireframeRenderer::Initialize();

      // Graphic settings
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glEnable( GL_BLEND );

      m_blockShader.Use();
      m_blockShader.Uniform("uBlockAtlas", 0);

      m_waterShader.Use();
      m_waterShader.Uniform("uBlockAtlas", 0);

      //// TODO: Remove
      // Shadows

    #ifdef GFX_ShadowMapping
      InitializeShadowMapping();
    #endif
    }

    void Renderer::RenderWorld(World::World &world) {
      m_blockShader.Use();
      m_blockShader.Uniform("uBlockAtlas", static_cast<int>(m_blockAtlasTexture.GetId()));
      m_blockShader.Uniform("uCameraPos", m_playerPosition);

      glm::vec3 playerPos = m_playerPosition;
      
      std::vector<World::Chunk *> translucentChunks;
      
      for (auto &[chunkPos, chunk] : world.GetChunks()) {
        if (world.IsChunkEmpty(chunkPos)) {
          chunk->ClearBuffers();
          continue;
        }

        if (chunk->HasTranslucentBlocks()) {
          translucentChunks.push_back(chunk.get());
        }

        glm::mat4 model { 1.0f };
        model = glm::translate(model, glm::vec3(chunkPos.x * CHUNK_WIDTH, 0.0f, chunkPos.y * CHUNK_LENGTH));

        if (chunk->IsDirty()) {
          chunk->BufferVertices();

          chunk->SetDirty(false);
        }

        RenderMesh(chunk->GetMesh(), m_blockShader, model);
      }

      glm::vec2 playerChunkPos = world.GetChunkPosFromCoords(playerPos);

      std::ranges::sort(translucentChunks, [playerChunkPos](World::Chunk *a, World::Chunk *b) {
        float distanceA = glm::distance(playerChunkPos, static_cast<glm::vec2>(a->GetChunkPos()));
        float distanceB = glm::distance(playerChunkPos, static_cast<glm::vec2>(b->GetChunkPos()));

        return distanceA > distanceB;
      });

      for (auto chunk : translucentChunks) {

        //TODO: abstract to mesh
        if (chunk->IsTranslucentDirty()) {
          chunk->BufferTranslucentVertices();
          chunk->SetTranslucentDirty(false);
        }

        const glm::ivec2 chunkPos = chunk->GetChunkPos();

        glm::mat4 model { 1.0f };
        model = glm::translate(model, glm::vec3(chunkPos.x * CHUNK_WIDTH, 0.0f, chunkPos.y * CHUNK_LENGTH));

        glDepthMask(GL_FALSE);
        RenderMesh(chunk->GetTranslucentMesh(), m_blockShader, model);
        glDepthMask(GL_TRUE);
      }
    }

    void Renderer::RenderMesh(Geometry::Mesh &mesh, Shader &shader, glm::mat4 &model) {
      if (mesh.GetVertexCount() == 0)
        return;

      shader.Use();
      shader.Uniform("uModel", model);

      mesh.BindVertexArray();
      glDrawElements(GL_TRIANGLES, mesh.GetVertexCount(), GL_UNSIGNED_INT, nullptr);
    }

    void Renderer::RenderUI(UI::UserInterface &ui) {
      ui.Draw();
    }

    void Renderer::Begin3D(const std::shared_ptr<Scene::PlayerCamera> &camera3D) {
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      glFrontFace(GL_CCW);

      glEnable(GL_DEPTH_TEST);

      if (m_isWireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }

      m_currentCamera = camera3D;

      glEnable(GL_FRAMEBUFFER_SRGB); 

      m_blockShader.Use();
      m_blockShader.Uniform("uViewProjection", m_currentCamera->GetViewProjection());

      m_waterShader.Use();
      m_waterShader.Uniform("uViewProjection", m_currentCamera->GetViewProjection());
    }

    void Renderer::End3D() {
      glDisable(GL_CULL_FACE);
      glDisable(GL_DEPTH_TEST);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      glDisable(GL_FRAMEBUFFER_SRGB); 

      m_currentCamera.reset();
    }

    auto Renderer::HasCamera() const -> bool {
      return m_currentCamera != nullptr;
    }

    void Renderer::ClearBackground(const glm::vec3 &color) const {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClearColor(color.r, color.g, color.b, 1.f);
    }

    void Renderer::ToggleWireframeMode() {
      m_isWireframeMode = !m_isWireframeMode;
    }

  }

}
