#include "Graphics/Renderer.h"
#include "Utils/Profiler.h"
#include "World/Chunk.h"
#include "Graphics/gfx.h"

namespace Graphics {

void Renderer::Initialize(float width, float height) {
  PROFILE_SCOPE(Graphics, "Renderer::Initialize")

  viewportWidth = width;
  viewportHeight = height;  

  //// Block Setup
  blockAtlasTexture.Load("../resources/textures/block_atlas.png", 0);
  
  blockShader.Load("../resources/shaders/block.vs", "../resources/shaders/block.fs");

  blockShader.Use();
  blockShader.Uniform("uBlockAtlas", 0);

  //// UI Setup
  fontMap.Load("../resources/font/pixel_2w.png", 1);

  uiShader.Load("../resources/shaders/ui.vs", "../resources/shaders/ui.fs");
  textShader.Load("../resources/shaders/text.vs", "../resources/shaders/text.fs");

  uiShader.Use();
  const glm::mat4 projection = glm::ortho(0.0f, viewportWidth, viewportHeight, 0.0f);
  uiShader.Uniform("uProjection", projection);

  textShader.Use();
  textShader.Uniform("uProjection", projection);
  textShader.Uniform("uFontMap", 1);
}

void Renderer::RenderWorld(World::World &world) {
  PROFILE_FUNCTION(World)

  for (auto &[offset, chunk] : world.GetChunks()) {
    if (!chunk.IsHidden() && chunk.GetState() == World::ChunkState::Loaded) {
      chunk.Render(blockAtlasTexture, blockShader, currentCamera->GetPosition());
    }
  }
}

void Renderer::RenderUI(UI::UserInterface &ui) {
  PROFILE_FUNCTION(UserInterface)
  
  const glm::mat4 projection = glm::ortho(0.0f, viewportWidth, viewportHeight, 0.0f);
  ui.SetProjection(projection);
  ui.Render(uiShader, textShader, fontMap);
}

void Renderer::Begin3D(const std::shared_ptr<Scene::Camera> &camera3D) {
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  if (isWireframeMode) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  currentCamera = camera3D;

  glEnable(GL_FRAMEBUFFER_SRGB); 

  blockShader.Use();
  blockShader.Uniform("uViewProjection", currentCamera->GetViewProjection());
}

void Renderer::End3D() {
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glDisable(GL_FRAMEBUFFER_SRGB); 


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
