#include "Renderer.h"
#include "gfx.h"
#include <random>

Renderer::Renderer(Window &window) : window(window), shader() {}

Renderer::~Renderer() {}

void Renderer::Init() {
  shader.Init("../resources/shaders/basic.vs", "../resources/shaders/basic.fs");

  const glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(window.GetWidth()), static_cast<float>(window.GetHeight()), 0.0f);

  shader.use();
  shader.uniform("uProjection", projection);

  // Set up buffers
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*) 0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void Renderer::DrawRectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat height, const glm::vec3 &color) {
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

void Renderer::StartDrawing() {
  glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::StopDrawing() {

  // Buffer data
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(GLfloat)), vertices.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)), indices.data(), GL_DYNAMIC_DRAW);

  // Render
  shader.use();
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  window.SwapBuffers();
  
  vertices.clear();
  indices.clear();
}

void Renderer::ClearBackground(const glm::vec3 &color) {
  glClearColor(color.r, color.g, color.b, 1.f);
}

