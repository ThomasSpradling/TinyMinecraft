#include "Graphics/Renderer.h"
#include "World/Chunk.h"
#include "Graphics/gfx.h"

namespace Graphics {

void Renderer::Init(float width, float height) {
  viewportWidth = width;
  viewportHeight = height;

  blockAtlasTexture.Load("../resources/textures/block_atlas.png");

  uiShader.Load("../resources/shaders/ui.vs", "../resources/shaders/ui.fs");
  blockShader.Load("../resources/shaders/block.vs", "../resources/shaders/block.fs");
}

void Renderer::RenderWorld(World::World &world) {
  for (auto &[offset, chunk] : world.GetChunks()) {
    if (!chunk.IsHidden() && chunk.GetState() == World::ChunkState::Loaded) {
      RenderChunk(chunk, offset);
    }
  }
}

void Renderer::RenderChunk(World::Chunk &chunk, const glm::vec2 &offset) {
  blockAtlasTexture.Bind();

  blockShader.Use();

  blockShader.Uniform("uBlockAtlas", 0);

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(offset.x * CHUNK_WIDTH, 0.0f, offset.y * CHUNK_LENGTH));
  blockShader.Uniform("uModel", model);

  glBindVertexArray(chunk.GetMeshVAO());
  glDrawElements(GL_TRIANGLES, chunk.GetVertexCount(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Renderer::RenderUI(UI::UserInterface &ui) {
  const glm::mat4 projection = glm::ortho(0.0f, viewportWidth, viewportHeight, 0.0f);

  uiShader.Use();
  uiShader.Uniform("uProjection", projection);

  glBindVertexArray(ui.GetVAO());
  glDrawElements(GL_TRIANGLES, ui.GetVertexCount(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Renderer::Begin3D(const std::shared_ptr<Scene::Camera> &camera3D) {
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glEnable(GL_DEPTH_TEST);

  if (isWireframeMode) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  currentCamera = camera3D;

  blockShader.Use();
  blockShader.Uniform("uViewProjection", currentCamera->GetViewProjection());
}

void Renderer::End3D() {
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  currentCamera.reset();
}

auto Renderer::HasCamera() const -> bool {
  return currentCamera != nullptr;
}

void Renderer::ClearBackground(const glm::vec3 &color) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(color.r, color.g, color.b, 1.f);
}

void Renderer::ToggleWireframeMode() {
  isWireframeMode = !isWireframeMode;
}

}
