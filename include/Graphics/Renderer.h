#ifndef RENDERER_H_
#define RENDERER_H_

#include "Geometry/Mesh.h"
#include "Graphics/Texture.h"
#include "Graphics/Shader.h"
#include "Scene/PlayerCameras.h"
#include "UI/UserInterface.h"
#include "World/World.h"
#include <memory>

namespace TinyMinecraft {

  namespace Graphics {

    class Renderer {
    public:
      Renderer(float viewportWidth, float viewportHeight);
      
      void RenderWorld(World::World &world);
      void RenderUI(UI::UserInterface &ui);
      void RenderMesh(Geometry::Mesh &mesh, Shader &shader, glm::mat4 &model);
      void RenderWireframeCube();

      void Begin3D(const std::shared_ptr<Scene::PlayerCamera> &camera3D);
      void End3D();
      [[nodiscard]] auto HasCamera() const -> bool;

      void ClearBackground(const glm::vec3 &color) const;
      void ToggleWireframeMode();

      inline void SetPlayerPosition(glm::vec3 &value) { m_playerPosition = value; }

    private:
      Shader m_blockShader, m_waterShader;
      Texture m_blockAtlasTexture;
      glm::vec3 m_playerPosition { 0.0f };
      std::shared_ptr<Scene::PlayerCamera> m_currentCamera = nullptr;

      float m_viewportWidth, m_viewportHeight;

      bool m_isWireframeMode = false;
    };

  }
}

#endif // RENDERER_H_