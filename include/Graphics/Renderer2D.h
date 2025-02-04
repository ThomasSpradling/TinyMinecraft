#ifndef RENDERER_2D_H_
#define RENDERER_2D_H_

#include <string>

#include "Graphics/OrthographicCamera.h"
#include "Geometry/geometry.h"

namespace TinyMinecraft {

  namespace Graphics {

    class Renderer2D {
    public:
      static void Initialize();
      static void Shutdown();
      
      static void BeginScene(const OrthographicCamera &camera);
      static void EndScene();

      static void DrawRectangle(float x, float y, float width, float height, const glm::vec4 &color);
      static void DrawRectangle(const Geometry::Rectangle &dest, const glm::vec4 &color);

      static void DrawLine(const glm::vec2 &point0, const glm::vec2 &point1, const glm::vec4 &color);
      static void DrawString(const std::string &text, float x, float y, const glm::vec4 &color, float scale = 1.0f);

      struct Statistics {
        int drawCalls = 0;
        int quadCount = 0;

        auto GetTotalVertexCount() -> int { return quadCount * 4; }
        auto GetTotalIndexCount() -> int { return quadCount * 6; }
      };
      
    private:
    };

  }

}

#endif // RENDERER_2D_H_
