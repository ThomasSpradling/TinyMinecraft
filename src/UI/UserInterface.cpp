#include "UI/UserInterface.h"
#include "Graphics/gfx.h"
#include "Utils/defs.h"
#include "World/Biome.h"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace UI {

UserInterface::UserInterface()
  : m_basicVBO(GL_ARRAY_BUFFER)
  , m_textVBO(GL_ARRAY_BUFFER)
  , m_basicEBO(GL_ELEMENT_ARRAY_BUFFER)
  , m_textEBO(GL_ELEMENT_ARRAY_BUFFER)
{
  InitializeGeneral();
  InitializeText();
}

void UserInterface::Arrange() {
  std::ostringstream debug;

#ifdef UTILS_ShowFPS  // FPS
  debug << "CURRENT FPS: " << m_currentFPS << "\n";
#endif

  debug << std::fixed << std::setprecision(3);
  debug << "XYZ: " 
        << m_playerPosition.x << ", " 
        << m_playerPosition.y << ", "
        << m_playerPosition.z << "\n";
  debug << std::defaultfloat;

  debug << "Chunk: "
        << m_chunkPosition.x << ", "
        << m_chunkPosition.y << "\n\n";

  debug << std::fixed << std::setprecision(5);
  debug << "Climate: "
        << "T " << m_temperature << ", H " << m_humidity << "\n";

  debug << "Biome: "
        << World::Biome::GetBiomeName(m_biome) << "\n";

  DrawText(debug.str(), 10, 10, glm::vec3(1, 1, 1), 1);
  
  Update();
}

void UserInterface::Render(Graphics::Shader &uiShader, Graphics::Shader &textShader, Graphics::Texture &fontMap) {
  //// General
  uiShader.Use();
  m_basicVAO.Bind();

  glDrawElements(GL_TRIANGLES, static_cast<int>(m_basicVertexCount), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);

  //// Text
  textShader.Use();
  fontMap.Bind();

  m_textVAO.Bind();

  glDrawElements(GL_TRIANGLES, static_cast<int>(m_textVertexCount), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

void UserInterface::SetClimateValues(float temperature, float humidity, World::BiomeType biome) {
  m_temperature = temperature;
  m_humidity = humidity;
  m_biome = biome;
}

void UserInterface::Update() {
  UpdateHelper(m_basicVAO, m_basicVBO, m_basicEBO, m_basicVertices, m_basicIndices);
  UpdateHelper(m_textVAO, m_textVBO, m_textEBO, m_textVertices, m_textIndices);

  m_basicVertexCount = m_basicIndices.size();
  m_textVertexCount = m_textIndices.size();

  m_basicVertices.clear();
  m_basicIndices.clear();

  m_textVertices.clear();
  m_textIndices.clear();
}

void UserInterface::InitializeGeneral() {
  m_basicVAO.Bind();
  m_basicVBO.Bind();
  m_basicEBO.Bind();

  m_basicVAO.AddAttribute(0, 2, GL_FLOAT, 5 * sizeof(GLfloat), 0);
  m_basicVAO.AddAttribute(1, 2, GL_FLOAT, 5 * sizeof(GLfloat), 2 * sizeof(GLfloat));
}

void UserInterface::InitializeText() {
  m_textVAO.Bind();
  m_textVBO.Bind();
  m_textEBO.Bind();

  m_textVAO.AddAttribute(0, 2, GL_FLOAT, 7 * sizeof(GLfloat), 0);
  m_textVAO.AddAttribute(1, 3, GL_FLOAT, 7 * sizeof(GLfloat), 2 * sizeof(GLfloat));
  m_textVAO.AddAttribute(2, 2, GL_FLOAT, 7 * sizeof(GLfloat), 5 * sizeof(GLfloat));
}

void UserInterface::SetCurrentFPS(int fps) {
  m_currentFPS = fps;
}

void UserInterface::SetPlayerPosition(const glm::vec3 &pos) {
  m_playerPosition = pos;
}

void UserInterface::SetChunkPosition(const glm::ivec2 &pos) {
  m_chunkPosition = pos;
}

void UserInterface::DrawRectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat height, const glm::vec3 &color) {
  GLuint n = m_basicVertices.size() / 5;   // index start

  // Setup geometry:
  
  m_basicVertices.insert(m_basicVertices.end(), {
    // position              // color
    x, y,                    color.r, color.g, color.b,      // top left
    x + width, y,            color.r, color.g, color.b,      // top right
    x, y + height,           color.r, color.g, color.b,      // bottom left
    x + width, y + height,   color.r, color.g, color.b      // bottom right
  });

  m_basicIndices.insert(m_basicIndices.end(), {
    n + 0, n + 1, n + 2,      // top left triangle
    n + 2, n + 3, n + 1       // bottom right triangle
  });
}

void UserInterface::UpdateHelper(Graphics::VertexArray &vao, Graphics::BufferObject &vbo, Graphics::BufferObject &ebo, std::vector<GLfloat> &verts, std::vector<GLuint> &indices) {
  vao.Bind();

  vbo.BufferData(verts, GL_DYNAMIC_DRAW);
  ebo.BufferData(indices, GL_DYNAMIC_DRAW);
}

void UserInterface::DrawText(const std::string &text, GLfloat x, GLfloat y, const glm::vec3 &color, float scale = 1) {
  std::string upperText = text;
  std::ranges::transform(upperText, upperText.begin(), ::toupper);

  GLuint n = m_textVertices.size() / 7;

  float offsetX = x;
  float offsetY = y;
  for (const char ch : upperText) { 
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

    // render it
    m_textVertices.insert(m_textVertices.end(), {
      offsetX, offsetY,
      color.r, color.g, color.b,
      texCoords[0].x, texCoords[0].y,

      offsetX + TEXT_CHAR_WIDTH * scale, offsetY,
      color.r, color.g, color.b,
      texCoords[1].x, texCoords[1].y,

      offsetX, offsetY + TEXT_CHAR_HEIGHT * scale,
      color.r, color.g, color.b,
      texCoords[2].x, texCoords[2].y,

      offsetX + TEXT_CHAR_WIDTH * scale,
      offsetY + TEXT_CHAR_HEIGHT * scale,
      color.r, color.g, color.b,texCoords[3].x, texCoords[3].y,
    });

    m_textIndices.insert(m_textIndices.end(), {
      n + 0, n + 1, n + 2,      // top left triangle
      n + 2, n + 3, n + 1       // bottom right triangle
    });

    n += 4;
    
    offsetX += TEXT_CHAR_WIDTH * scale;
  }
}

}
