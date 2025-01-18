#include "UI/UserInterface.h"

namespace UI {

void UserInterface::Initialize() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);

  // positions
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*) 0);
  glEnableVertexAttribArray(0);

  // colors
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void UserInterface::Arrange() {
  DrawRectangle(0, 0, 50, 50, glm::vec3(1.0f, 0.0f, 0.0f));
  
  Update();
}

void UserInterface::Update() {
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(GLfloat) * vertices.size()), vertices.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(GLuint) * indices.size()), indices.data(), GL_DYNAMIC_DRAW);

  glBindVertexArray(0);
}

GLuint UserInterface::GetVAO() {
  return VAO;
}

GLsizei UserInterface::GetVertexCount() {
  return vertices.size();
}

void UserInterface::DrawRectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat height, const glm::vec3 &color) {
  GLuint n = vertices.size() / 5;   // index start

  // Setup geometry:
  vertices.insert(vertices.end(), {
    // position              // color
    x, y,                    color.r, color.g, color.b,      // top left
    x + width, y,            color.r, color.g, color.b,      // top right
    x, y + height,           color.r, color.g, color.b,      // bottom left
    x + width, y + height,   color.r, color.g, color.b      // bottom right
  });

  indices.insert(indices.end(), {
    n + 0, n + 1, n + 2,      // top left triangle
    n + 2, n + 3, n + 1       // bottom right triangle
  });
}

}
