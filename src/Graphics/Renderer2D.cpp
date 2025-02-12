#include "Graphics/Renderer2D.h"
#include "Graphics/BufferObject.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/VertexArray.h"
#include "Graphics/gfx.h"
#include "Utils/defs.h"
#include <array>
#include <algorithm>
#include <cstddef>
#include <memory>
#include <ranges>

namespace TinyMinecraft {

  namespace Graphics {

    struct RectVertex {
      glm::vec2 position;
      glm::vec4 color;
    };

    struct LineVertex {
      glm::vec2 position;
      glm::vec4 color;
    };

    struct TextVertex {
      glm::vec2 position;
      glm::vec4 color;
      glm::vec2 texCoord;
    };

    struct Renderer2DData {
      static constexpr int MAX_QUADS = 20000;
      static constexpr int MAX_INDICES = MAX_QUADS * 6;
      static constexpr int MAX_VERTICES = MAX_QUADS * 4;

      int rectIndexCount = 0;
      int lineVertexCount = 0;
      int textIndexCount = 0;

      std::unique_ptr<VertexArray> rectVAO;
      std::unique_ptr<BufferObject> rectVBO;
      std::unique_ptr<Shader> rectShader;
      std::vector<RectVertex> rectVertices;

      std::unique_ptr<VertexArray> lineVAO;
      std::unique_ptr<BufferObject> lineVBO;
      std::unique_ptr<Shader> lineShader;
      std::vector<LineVertex> lineVertices;

      std::unique_ptr<VertexArray> textVAO;
      std::unique_ptr<BufferObject> textVBO;
      std::unique_ptr<Shader> textShader;
      std::vector<TextVertex> textVertices;

      std::unique_ptr<BufferObject> quadEBO;
      std::array<GLuint, MAX_INDICES> indices;

      std::unique_ptr<Texture> fontMap;

      glm::mat4 viewProjection;
      Renderer2D::Statistics stats;
    };

    static Renderer2DData s_data;

    void Renderer2D::Initialize() {
      // rect
      s_data.rectVAO = std::make_unique<VertexArray>();
      s_data.rectVBO = std::make_unique<BufferObject>(GL_ARRAY_BUFFER);
      s_data.quadEBO = std::make_unique<BufferObject>(GL_ELEMENT_ARRAY_BUFFER);

      s_data.rectVAO->AddAttribute(0, 2, GL_FLOAT, sizeof(RectVertex), offsetof(RectVertex, position));
      s_data.rectVAO->AddAttribute(1, 4, GL_FLOAT, sizeof(RectVertex), offsetof(RectVertex, color));

      s_data.rectShader = std::make_unique<Shader>("../resources/shaders/Renderer2D/Rect.vs", "../resources/shaders/Renderer2D/Rect.fs");

      // line
      s_data.lineVAO = std::make_unique<VertexArray>();
      s_data.lineVBO = std::make_unique<BufferObject>(GL_ARRAY_BUFFER);

      s_data.lineVAO->AddAttribute(0, 2, GL_FLOAT, sizeof(LineVertex), offsetof(LineVertex, position));
      s_data.lineVAO->AddAttribute(1, 4, GL_FLOAT, sizeof(LineVertex), offsetof(LineVertex, color));

      s_data.lineShader = std::make_unique<Shader>("../resources/shaders/Renderer2D/Line.vs", "../resources/shaders/Renderer2D/Line.fs");

      // text
      s_data.textVAO = std::make_unique<VertexArray>();
      s_data.textVBO = std::make_unique<BufferObject>(GL_ARRAY_BUFFER);

      s_data.textVAO->AddAttribute(0, 2, GL_FLOAT, sizeof(TextVertex), offsetof(TextVertex, position));
      s_data.textVAO->AddAttribute(1, 4, GL_FLOAT, sizeof(TextVertex), offsetof(TextVertex, color));
      s_data.textVAO->AddAttribute(2, 2, GL_FLOAT, sizeof(TextVertex), offsetof(TextVertex, texCoord));

      s_data.textShader = std::make_unique<Shader>("../resources/shaders/Renderer2D/Text.vs", "../resources/shaders/Renderer2D/Text.fs");
      s_data.fontMap = std::make_unique<Texture>("../resources/font/pixel_2w.png");
      s_data.fontMap->Bind();
      s_data.textShader->Uniform("u_FontMap", static_cast<int>(s_data.fontMap->GetId()));

      int offset = 0;
      for (int i = 0; i < Renderer2DData::MAX_INDICES; i += 6) {
        s_data.indices.at(i + 0) = offset + 0;
        s_data.indices.at(i + 1) = offset + 1;
        s_data.indices.at(i + 2) = offset + 2;

        s_data.indices.at(i + 3) = offset + 2;
        s_data.indices.at(i + 4) = offset + 3;
        s_data.indices.at(i + 5) = offset + 1;

        offset += 4;
      }
      
      s_data.quadEBO->BufferData(s_data.indices, GL_STATIC_DRAW);
    }

    void Renderer2D::BeginScene(const OrthographicCamera &camera) {
      // update to use UBO

      s_data.viewProjection = camera.GetViewProjectionMatrix();

      s_data.rectShader->Uniform("u_ViewProjection", s_data.viewProjection);
      s_data.lineShader->Uniform("u_ViewProjection", s_data.viewProjection);
      s_data.textShader->Uniform("u_ViewProjection", s_data.viewProjection);
    }

    void Renderer2D::EndScene() {
      glDisable(GL_CULL_FACE);
      
    #if DEBUG and not SILENT
      if (s_data.stats.quadCount > Renderer2DData::MAX_QUADS) {
        Utils::Logger::Warning("You are drawing too many quads! Some quads may not get rendered. Current quad count: {} / {}.", s_data.stats.quadCount, Renderer2DData::MAX_QUADS);
      }
    #endif

      if (s_data.rectIndexCount > 0) {
        s_data.rectVAO->Bind();
        s_data.rectShader->Use();

        s_data.rectVBO->BufferData(s_data.rectVertices, GL_DYNAMIC_DRAW);

        glDrawElements(GL_TRIANGLES, s_data.rectIndexCount, GL_UNSIGNED_INT, nullptr);
        s_data.rectVertices.clear();

        s_data.stats.drawCalls++;
      }

      if (s_data.lineVertexCount > 0) {
        s_data.lineVAO->Bind();
        s_data.lineShader->Use();

        s_data.lineVBO->BufferData(s_data.lineVertices, GL_DYNAMIC_DRAW);

        glDrawArrays(GL_LINES, 0, s_data.lineVertexCount);
        s_data.lineVertices.clear();

        s_data.stats.drawCalls++;
      }

      if (s_data.textIndexCount > 0) {
        s_data.fontMap->Bind();
        s_data.textVAO->Bind();
        s_data.textShader->Use();

        s_data.textVBO->BufferData(s_data.textVertices, GL_DYNAMIC_DRAW);

        glDrawElements(GL_TRIANGLES, s_data.textIndexCount, GL_UNSIGNED_INT, nullptr);
        s_data.textVertices.clear();

        s_data.stats.drawCalls++;
      }

      s_data.stats.quadCount = 0;
      s_data.rectIndexCount = 0;
      s_data.lineVertexCount = 0;
      s_data.textIndexCount = 0;

      glEnable(GL_CULL_FACE);
    }

    void Renderer2D::DrawRectangle(float x, float y, float width, float height, const glm::vec4 &color) {
      DrawRectangle(Geometry::Rectangle{x, y, width, height}, color);
    }

    void Renderer2D::DrawRectangle(const Geometry::Rectangle &dest, const glm::vec4 &color) {
      s_data.rectVertices.insert(s_data.rectVertices.begin(), {
        { glm::vec2(dest.x, dest.y),                              color },
        { glm::vec2(dest.x + dest.width, dest.y),                 color },
        { glm::vec2(dest.x, dest.y + dest.height),                color },
        { glm::vec2(dest.x + dest.width, dest.y + dest.height),   color },
      });

      s_data.rectIndexCount += 6;
      s_data.stats.quadCount++;
    }

    void Renderer2D::DrawLine(const glm::vec2 &point0, const glm::vec2 &point1, const glm::vec4 &color) {
      s_data.lineVertices.insert(s_data.lineVertices.begin(), {
        { point0, color },
        { point1, color },
      });
    }

    void Renderer2D::DrawString(const std::string &text, float x, float y, const glm::vec4 &color, float scale) {
      std::string uppercaseText = text;
      std::ranges::transform(uppercaseText, uppercaseText.begin(), ::toupper);
      
      float offsetX = x;
      float offsetY = y;

      for (const char ch : uppercaseText) {
        if (ch == '\n') {
          offsetY += TEXT_CHAR_HEIGHT * scale + 5 * scale;
          offsetX = x;
          continue;
        }

        if (ch < 32 || ch >= 96) {
          continue;
        }

        // subtract by ASCII value for space. FontMap is arrange in order afterward
        const int indexX = (ch - 32) % (FONT_MAP_WIDTH / TEXT_CHAR_WIDTH);
        const int indexY = (ch - 32) / (FONT_MAP_WIDTH / TEXT_CHAR_WIDTH);

        static constexpr glm::vec2 tileSize = glm::vec2(
          static_cast<float>(TEXT_CHAR_WIDTH) / static_cast<float>(FONT_MAP_WIDTH),
          static_cast<float>(TEXT_CHAR_HEIGHT) / static_cast<float>(FONT_MAP_HEIGHT)
        );

        glm::vec2 topLeftTexCoord = glm::vec2(tileSize.x * static_cast<float>(indexX), tileSize.y * static_cast<float>(indexY));

        std::array<glm::vec2, 4> texCoords = {
            topLeftTexCoord,
            { topLeftTexCoord.x + tileSize.x, topLeftTexCoord.y },
            { topLeftTexCoord.x, topLeftTexCoord.y + tileSize.y },
            { topLeftTexCoord.x + tileSize.x, topLeftTexCoord.y + tileSize.y }
        };

        float width = TEXT_CHAR_WIDTH * scale;
        float height = TEXT_CHAR_HEIGHT * scale;

        s_data.textVertices.insert(s_data.textVertices.begin(), {
          { glm::vec2(offsetX, offsetY),                    color,    texCoords[0] },
          { glm::vec2(offsetX + width, offsetY),            color,    texCoords[1] },
          { glm::vec2(offsetX, offsetY + height),           color,    texCoords[2] },
          { glm::vec2(offsetX + width, offsetY + height),   color,    texCoords[3] },
        });
        
        offsetX += TEXT_CHAR_WIDTH * scale;

        s_data.textIndexCount += 6;
        s_data.stats.quadCount++;
      }

    }

  }

}
