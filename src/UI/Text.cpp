#include "UI/Text.h"
#include "Utils/defs.h"
#include <cstring>
#include <iostream>

namespace UI {

void Text::Initialize() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);

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

void Text::Draw(const std::string &text, GLfloat x, GLfloat y, const glm::vec3 &color) {
  GLuint n = vertices.size() / 7;

  float offsetX = x;
  float offsetY = y;
  for (const char ch : text) { 
    if (ch == '\n') {
      offsetY += TEXT_CHAR_HEIGHT + 5;
      offsetX = x;
      continue;
    }

    if (ch < 32 || ch >= 96) {
      continue;
    }

    // subtract by ASCII value for space. FontMap is arrange in order afterward
    const int indexX = (ch - 32) % (FONT_MAP_WIDTH / TEXT_CHAR_WIDTH);
    const int indexY = (ch - 32) / (FONT_MAP_WIDTH / TEXT_CHAR_WIDTH);

    glm::vec2 topLeftTexCoord = glm::vec2(tileSize.x * indexX, tileSize.y * indexY);

    std::array<glm::vec2, 4> texCoords = {
        topLeftTexCoord,
        { topLeftTexCoord.x + tileSize.x, topLeftTexCoord.y },
        { topLeftTexCoord.x, topLeftTexCoord.y + tileSize.y },
        { topLeftTexCoord.x + tileSize.x, topLeftTexCoord.y + tileSize.y }
    };

    std::cout << "XX: " << topLeftTexCoord.x << std::endl;

    std::cout << "OFFSET: " << offsetX << "; " << offsetY << std::endl;

    // render it
    vertices.insert(vertices.end(), {
      // position                                    // color                       // texture coords
      offsetX, offsetY,                                          color.r, color.g, color.b,     texCoords[0].x, texCoords[0].y,
      offsetX + TEXT_CHAR_WIDTH, offsetY,                        color.r, color.g, color.b,    texCoords[1].x, texCoords[1].y,
      offsetX, offsetY + TEXT_CHAR_HEIGHT,                       color.r, color.g, color.b,     texCoords[2].x, texCoords[2].y,
      offsetX + TEXT_CHAR_WIDTH, offsetY + TEXT_CHAR_HEIGHT,     color.r, color.g, color.b,     texCoords[3].x, texCoords[3].y,
    });

    indices.insert(indices.end(), {
      n + 0, n + 1, n + 2,      // top left triangle
      n + 2, n + 3, n + 1       // bottom right triangle
    });

    n += 4;
    
    offsetX += TEXT_CHAR_WIDTH;
  }
}

void Text::Update() {
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(GLfloat) * vertices.size()), vertices.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(GLuint) * indices.size()), indices.data(), GL_DYNAMIC_DRAW);

  glBindVertexArray(0);
}

GLuint Text::GetVAO() {
  return VAO;
}

GLsizei Text::GetVertexCount() {
  std::cout << "SIZE::: " <<vertices.size() <<std::endl;
  return vertices.size();
}

}
