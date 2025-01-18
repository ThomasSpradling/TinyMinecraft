#ifndef RENDERER_H_
#define RENDERER_H_

#include "Scene/Camera.h"
#include "Graphics/Shader.h"
#include "UI/UserInterface.h"
#include "World/World.h"
#include "Graphics/gfx.h"
#include <memory>

namespace Graphics {

class Renderer {
public:
  Renderer() = default;
  ~Renderer() = default;

  void Init(float viewportWidth, float viewportHeight);
  
  void RenderWorld(World::World &world);
  void RenderChunk(World::Chunk &chunk);
  void RenderUI(UI::UserInterface &ui);

  void Begin3D(const std::shared_ptr<Scene::Camera> &camera3D);
  void End3D();
  auto HasCamera() const -> bool;

  void ClearBackground(const glm::vec3 &color);
  void ToggleWireframeMode();

private:
  Shader uiShader, blockShader;

  float viewportWidth, viewportHeight;

  std::shared_ptr<Scene::Camera> currentCamera = nullptr;

  bool isWireframeMode = false;

};

}

#endif // RENDERER_H_