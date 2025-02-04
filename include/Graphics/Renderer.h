#ifndef RENDERER_H_
#define RENDERER_H_

#include "Geometry/Mesh.h"
#include "Graphics/Texture.h"
#include "Scene/Camera.h"
#include "Graphics/Shader.h"
#include "UI/UserInterface.h"
#include "World/World.h"
#include <memory>

namespace TinyMinecraft {

  namespace Graphics {

    class Renderer {
    public:
      Renderer(float viewportWidth, float viewportHeight);
      
      // void RenderShadows(World::World &world);
      void RenderWorld(World::World &world);
      void RenderUI(UI::UserInterface &ui);
      void RenderMesh(Geometry::Mesh &mesh, Shader &shader, glm::mat4 &model);

      void Begin3D(const std::shared_ptr<Scene::Camera> &camera3D);
      void End3D();
      [[nodiscard]] auto HasCamera() const -> bool;

      void ClearBackground(const glm::vec3 &color) const;
      void ToggleWireframeMode();

    private:
      Shader m_blockShader, m_waterShader;
      Texture m_blockAtlasTexture;
      std::shared_ptr<Scene::Camera> m_currentCamera = nullptr;

      float m_viewportWidth, m_viewportHeight;

      bool m_isWireframeMode = false;
    };

  }
}

#endif // RENDERER_H_