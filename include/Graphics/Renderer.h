#ifndef RENDERER_H_
#define RENDERER_H_

#include "Graphics/Texture.h"
#include "Scene/Camera.h"
#include "Graphics/Shader.h"
#include "UI/UserInterface.h"
#include "World/World.h"
#include <memory>

namespace Graphics {

class Renderer {
public:
  Renderer(float viewportWidth, float viewportHeight);
  
  // void RenderShadows(World::World &world);
  void RenderWorld(World::World &world);
  void RenderUI(UI::UserInterface &ui);

  void Begin3D(const std::shared_ptr<Scene::Camera> &camera3D);
  void End3D();
  [[nodiscard]] auto HasCamera() const -> bool;

  void ClearBackground(const glm::vec3 &color) const;
  void ToggleWireframeMode();

private:
  Shader m_uiShader, m_blockShader, m_waterShader, m_textShader, m_depthShader, m_debugDepthQuadShader;
  Texture m_blockAtlasTexture, m_fontMap;
  std::shared_ptr<Scene::Camera> m_currentCamera = nullptr;

  /// TODO: Remove
  // GLuint m_depthMapFBO, m_depthMap;
  // float m_shadowMapWidth, m_shadowMapHeight;

  float m_viewportWidth, m_viewportHeight;

  bool m_isWireframeMode = false;

  // void InitializeShadowMapping();
};

}

#endif // RENDERER_H_