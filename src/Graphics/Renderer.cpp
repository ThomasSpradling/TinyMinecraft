#include "Graphics/Renderer.h"
#include "Utils/Profiler.h"
#include "World/Chunk.h"
#include "Graphics/gfx.h"

namespace Graphics {

Renderer::Renderer(float viewportWidth, float viewportHeight)
  : m_uiShader("../resources/shaders/ui.vs", "../resources/shaders/ui.fs")
  , m_blockShader("../resources/shaders/block_solid.vs", "../resources/shaders/block_solid.fs")
  , m_waterShader("../resources/shaders/block_translucent.vs", "../resources/shaders/block_translucent.fs")
  , m_textShader("../resources/shaders/text.vs", "../resources/shaders/text.fs")
  , m_depthShader("../resources/shaders/depth.vs", "../resources/shaders/depth.fs")
  , m_debugDepthQuadShader("../resources/shaders/debug_quad.vs", "../resources/shaders/debug_quad.fs")
  , m_blockAtlasTexture("../resources/textures/block_atlas.png")
  , m_fontMap("../resources/font/pixel_2w.png")
  , m_viewportWidth(viewportWidth)
  , m_viewportHeight(viewportHeight)
{
  PROFILE_SCOPE(Graphics, "Renderer::Initialize") 

  // Graphic settings
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable( GL_BLEND );

  m_blockShader.Use();
  m_blockShader.Uniform("uBlockAtlas", 0);

  m_waterShader.Use();
  m_waterShader.Uniform("uBlockAtlas", 0);

  m_uiShader.Use();
  const glm::mat4 projection = glm::ortho(0.0f, viewportWidth, viewportHeight, 0.0f);
  m_uiShader.Uniform("uProjection", projection);

  m_textShader.Use();
  m_textShader.Uniform("uProjection", projection);
  m_textShader.Uniform("uFontMap", 1);

  //// TODO: Remove
  // Shadows

#ifdef GFX_ShadowMapping
  InitializeShadowMapping();
#endif
}

void Renderer::RenderWorld(World::World &world) {
  world.Render(m_blockAtlasTexture, m_blockShader, m_waterShader, m_currentCamera->GetPosition());
}

// void Renderer::RenderShadows(World::World &world) {
//   float near_plane = 1.f, far_plane = 100.f;

//   // TODO: Fix this!!
//   glm::mat4 lightProjection, lightView;
//   glm::mat4 lightViewProjection;

//   // float chunkDistance = 12;

//   lightProjection = glm::ortho(-100.f, 100.f, -100.f, 100.f, near_plane, far_plane);
//   lightView = glm::lookAt(glm::vec3(-40, 80, -50), glm::vec3(0.f), glm::vec3(0.0, 1.0, 0.0));
//   lightViewProjection = lightProjection * lightView;

//   m_depthShader.Use();
//   m_depthShader.Uniform("uLightViewProjection", lightViewProjection);
  
//   glViewport(0, 0, m_shadowMapWidth, m_shadowMapHeight);
//   glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
//     glClear(GL_DEPTH_BUFFER_BIT);
//     m_blockAtlasTexture.Bind();
//     for (auto &[offset, chunk] : world.GetLoadedChunks()) {
//       if (!chunk.IsHidden() && chunk.GetState() == World::ChunkState::Loaded) {
//         // chunk.Render(m_blockAtlasTexture, depthMap, m_depthShader, currentCamera->GetPosition());
//       }
//     }
//   glBindFramebuffer(GL_FRAMEBUFFER, 0);

//   m_blockShader.Use();
//   m_blockShader.Uniform("uLightViewProjection", lightViewProjection);

//   glViewport(0, 0, m_viewportWidth * 2, m_viewportHeight * 2);
//   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//   // m_debugDepthQuad.Use();
//   // m_debugDepthQuad.Uniform("uNearPlane", near_plane);
//   // m_debugDepthQuad.Uniform("uFarPlane", far_plane);
//   // glActiveTexture(GL_TEXTURE2);
//   // glBindTexture(GL_TEXTURE_2D, depthMap);

//   // // render onto quad
//   // renderQuad();
// }

void Renderer::RenderUI(UI::UserInterface &ui) {
  PROFILE_FUNCTION(UserInterface)
  
  ui.Render(m_uiShader, m_textShader, m_fontMap);
}

void Renderer::Begin3D(const std::shared_ptr<Scene::Camera> &camera3D) {
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

// void Renderer::InitializeShadowMapping() {
//   m_blockShader.Use();
//   m_blockShader.Uniform("uShadowMap", 2);

//   glGenFramebuffers(1, &m_depthMapFBO);

//   m_shadowMapWidth = 4096;
//   m_shadowMapHeight = 4096;
//   glGenTextures(1, &m_depthMap);
//   glBindTexture(GL_TEXTURE_2D, m_depthMap);
//   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_shadowMapWidth, m_shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

//   float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
//   glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

//   glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
//     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
//     if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
//         std::cerr << "ERROR: Framebuffer is not complete!" << std::endl;
//     }

//     glDrawBuffer(GL_NONE);
//     glReadBuffer(GL_NONE);
//   glBindFramebuffer(GL_FRAMEBUFFER, 0);

//   m_debugDepthQuadShader.Use();
//   m_debugDepthQuadShader.Uniform("uDepthMap", 2);
// }

}
