#include "UI/UserInterface.h"
#include "Utils/defs.h"
#include <iomanip>
#include <sstream>

namespace UI {

void UserInterface::Initialize() {
  InitializeGeneral();
  InitializeText();
}

void UserInterface::Arrange() {
  std::ostringstream debug;

#ifdef UTILS_ShowFPS  // FPS
  debug << "CURRENT FPS: " << currentFPS << "\n";
#endif

  debug << std::fixed << std::setprecision(3);
  debug << "XYZ: " 
        << playerPosition.x << ", " 
        << playerPosition.y << ", "
        << playerPosition.z << "\n";
  debug << std::defaultfloat;

  debug << "CHUNK: "
        << chunkPosition.x << ", "
        << chunkPosition.y;

  DrawText(debug.str(), 10, 10, glm::vec3(1, 1, 1), 1);
  
  Update();
}

void UserInterface::Render(Graphics::Shader &uiShader, Graphics::Shader &textShader, Graphics::Texture &fontMap) {
  //// General
  uiShader.Use();
  glBindVertexArray(basicVAO);
  glDrawElements(GL_TRIANGLES, basicVertexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  //// Text
  textShader.Use();
  fontMap.Bind(1);
  glBindVertexArray(textVAO);
  glDrawElements(GL_TRIANGLES, textVertexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void UserInterface::Update() {
  UpdateHelper(basicVAO, basicVBO, basicEBO, basicVertices, basicIndices);
  UpdateHelper(textVAO, textVBO, textEBO, textVertices, textIndices);

  basicVertexCount = basicIndices.size();
  textVertexCount = textIndices.size();

  basicVertices.clear();
  basicIndices.clear();

  textVertices.clear();
  textIndices.clear();
}

void UserInterface::InitializeGeneral() {
  glGenVertexArrays(1, &basicVAO);
  glGenBuffers(1, &basicVBO);
  glGenBuffers(1, &basicEBO);

  glBindVertexArray(basicVAO);

  glBindBuffer(GL_ARRAY_BUFFER, basicVBO);
  glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, basicEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

  // positions
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*) 0);
  glEnableVertexAttribArray(0);

  // colors
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void UserInterface::InitializeText() {
  glGenVertexArrays(1, &textVAO);
  glGenBuffers(1, &textVBO);
  glGenBuffers(1, &textEBO);

  glBindVertexArray(textVAO);

  glBindBuffer(GL_ARRAY_BUFFER, textVBO);
  glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

  // positions
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*) 0);
  glEnableVertexAttribArray(0);

  // colors
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  // texture coordinates
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*) (5 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}

void UserInterface::SetCurrentFPS(int fps) {
  currentFPS = fps;
}

void UserInterface::SetPlayerPosition(const glm::vec3 &pos) {
  playerPosition = pos;
}

void UserInterface::SetChunkPosition(const glm::ivec2 &pos) {
  chunkPosition = pos;
}

void UserInterface::DrawRectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat height, const glm::vec3 &color) {
  GLuint n = basicVertices.size() / 5;   // index start

  // Setup geometry:
  
  basicVertices.insert(basicVertices.end(), {
    // position              // color
    x, y,                    color.r, color.g, color.b,      // top left
    x + width, y,            color.r, color.g, color.b,      // top right
    x, y + height,           color.r, color.g, color.b,      // bottom left
    x + width, y + height,   color.r, color.g, color.b      // bottom right
  });

  basicIndices.insert(basicIndices.end(), {
    n + 0, n + 1, n + 2,      // top left triangle
    n + 2, n + 3, n + 1       // bottom right triangle
  });
}

void UserInterface::UpdateHelper(GLuint VAO, GLuint VBO, GLuint EBO, std::vector<GLfloat> &verts, std::vector<GLuint> &indices) {
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(GLfloat) * verts.size()), verts.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(GLuint) * indices.size()), indices.data(), GL_DYNAMIC_DRAW);

  glBindVertexArray(0);
}

void UserInterface::DrawText(const std::string &text, GLfloat x, GLfloat y, const glm::vec3 &color, float scale = 1) {
  GLuint n = textVertices.size() / 7;

  float offsetX = x;
  float offsetY = y;
  for (const char ch : text) { 
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

    glm::vec2 topLeftTexCoord = glm::vec2(tileSize.x * indexX, tileSize.y * indexY);

    std::array<glm::vec2, 4> texCoords = {
        topLeftTexCoord,
        { topLeftTexCoord.x + tileSize.x, topLeftTexCoord.y },
        { topLeftTexCoord.x, topLeftTexCoord.y + tileSize.y },
        { topLeftTexCoord.x + tileSize.x, topLeftTexCoord.y + tileSize.y }
    };

    // render it
    textVertices.insert(textVertices.end(), {
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

    textIndices.insert(textIndices.end(), {
      n + 0, n + 1, n + 2,      // top left triangle
      n + 2, n + 3, n + 1       // bottom right triangle
    });

    n += 4;
    
    offsetX += TEXT_CHAR_WIDTH * scale;
  }
}

}
