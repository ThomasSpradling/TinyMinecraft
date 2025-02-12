#ifndef WIREFRAME_H_
#define WIREFRAME_H_

#include "Scene/PlayerCameras.h"

namespace TinyMinecraft {

  namespace Graphics {

    class WireframeRenderer {
    public:
      static void Initialize();
      static void Render(const std::shared_ptr<Scene::PlayerCamera> &camera);

      static void Draw3DRectangle(glm::vec3 start, glm::vec3 end, glm::vec4 color);
      static void DrawBlock(glm::vec3 pos, glm::vec4 color);
      static void DrawChunk(glm::ivec2 chunkPos, glm::vec4 color);
    private:
      static std::shared_ptr<Scene::PlayerCamera> s_camera;
    };
  }

}

#endif // WIREFRAME_H_