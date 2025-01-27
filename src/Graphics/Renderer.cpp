#include "Graphics/Renderer.h"
#include "Utils/Profiler.h"
#include "Utils/defs.h"
#include "World/Chunk.h"
#include "Graphics/gfx.h"
#include <iostream>

namespace Graphics {

void Renderer::Initialize(float width, float height) {
  PROFILE_SCOPE(Graphics, "Renderer::Initialize")

  viewportWidth = width;
  viewportHeight = height;  

  // Graphic settings
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable( GL_BLEND );

  //// Block Setup
  blockAtlasTexture.Load("../resources/textures/block_atlas.png", 0);

  blockShader.Load("../resources/shaders/block_solid.vs", "../resources/shaders/block_solid.fs");
  waterShader.Load("../resources/shaders/block_translucent.vs", "../resources/shaders/block_translucent.fs");
  debugDepthQuad.Load("../resources/shaders/debug_quad.vs", "../resources/shaders/debug_quad.fs");

  blockShader.Use();
  blockShader.Uniform("uBlockAtlas", 0);

  waterShader.Use();
  waterShader.Uniform("uBlockAtlas", 0);

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

  //// TODO: Remove
  // Shadows

#ifdef GFX_ShadowMapping
  InitializeShadowMapping();
#endif
}

void Renderer::RenderWorld(World::World &world) {
  world.Render(blockAtlasTexture, depthMap, blockShader, waterShader, currentCamera->GetPosition());
}

/// TODO: REMVOE


unsigned int quadVAO = 0;
unsigned int quadVBO;

void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    
    }

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

///

void Renderer::RenderShadows(World::World &world) {
  float near_plane = 1.f, far_plane = 100.f;

  // TODO: Fix this!!
  glm::mat4 lightProjection, lightView;
  glm::mat4 lightViewProjection;

  // float chunkDistance = 12;

  lightProjection = glm::ortho(-100.f, 100.f, -100.f, 100.f, near_plane, far_plane);
  lightView = glm::lookAt(glm::vec3(-40, 80, -50), glm::vec3(0.f), glm::vec3(0.0, 1.0, 0.0));
  lightViewProjection = lightProjection * lightView;

  depthShader.Use();
  depthShader.Uniform("uLightViewProjection", lightViewProjection);
  
  glViewport(0, 0, shadowMapWidth, shadowMapHeight);
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    blockAtlasTexture.Bind(0);
    for (auto &[offset, chunk] : world.GetLoadedChunks()) {
      if (!chunk.IsHidden() && chunk.GetState() == World::ChunkState::Loaded) {
        // chunk.Render(blockAtlasTexture, depthMap, depthShader, currentCamera->GetPosition());
      }
    }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  blockShader.Use();
  blockShader.Uniform("uLightViewProjection", lightViewProjection);

  glViewport(0, 0, viewportWidth * 2, viewportHeight * 2);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // debugDepthQuad.Use();
  // debugDepthQuad.Uniform("uNearPlane", near_plane);
  // debugDepthQuad.Uniform("uFarPlane", far_plane);
  // glActiveTexture(GL_TEXTURE2);
  // glBindTexture(GL_TEXTURE_2D, depthMap);

  // // render onto quad
  // renderQuad();
}

void Renderer::RenderUI(UI::UserInterface &ui) {
  PROFILE_FUNCTION(UserInterface)
  
  ui.Render(uiShader, textShader, fontMap);
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

  glEnable(GL_FRAMEBUFFER_SRGB); 

  blockShader.Use();
  blockShader.Uniform("uViewProjection", currentCamera->GetViewProjection());

  waterShader.Use();
  waterShader.Uniform("uViewProjection", currentCamera->GetViewProjection());
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

void Renderer::InitializeShadowMapping() {
  depthShader.Load("../resources/shaders/depth.vs", "../resources/shaders/depth.fs");

  blockShader.Use();
  blockShader.Uniform("uShadowMap", 2);

  glGenFramebuffers(1, &depthMapFBO);

  shadowMapWidth = 4096;
  shadowMapHeight = 4096;
  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR: Framebuffer is not complete!" << std::endl;
    }

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  debugDepthQuad.Use();
  debugDepthQuad.Uniform("uDepthMap", 2);
}

}
