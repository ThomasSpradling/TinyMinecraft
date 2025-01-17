#include "Renderer.h"
#include "gfx.h"
#include <random>

Renderer::Renderer(Window &window) : window(window), basicShader(), modelShader() {}

Renderer::~Renderer() {}

void Renderer::Init() {

  ///// Basic 2D Shader setup
  basicShader.Init("../resources/shaders/basic.vs", "../resources/shaders/basic.fs");

  const glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(window.GetWidth()), static_cast<float>(window.GetHeight()), 0.0f);

  basicShader.use();
  basicShader.uniform("uProjection", projection);

  // Set up buffers
  glGenVertexArrays(1, &basicVAO);
  glGenBuffers(1, &basicVBO);
  glGenBuffers(1, &basicEBO);

  glBindVertexArray(basicVAO);

  glBindBuffer(GL_ARRAY_BUFFER, basicVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, basicEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);

  // positions
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*) 0);
  glEnableVertexAttribArray(0);

  // colors
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  ///// Basic 3D Shader setup
  modelShader.Init("../resources/shaders/models.vs", "../resources/shaders/models.fs");

  // Set up buffers
  glGenVertexArrays(1, &modelVAO);
  glGenBuffers(1, &modelVBO);

  glBindVertexArray(modelVAO);

  glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

  // positions
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*) 0);
  glEnableVertexAttribArray(0);

  // colors
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*) (3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  // normals
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*) (6 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}


void Renderer::StartDrawing() {
  glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::StopDrawing() {
  //// 3D
  glBindVertexArray(modelVAO);

  glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(modelVertices.size() * sizeof(GLfloat)), modelVertices.data(), GL_DYNAMIC_DRAW);

  modelShader.use();
  glDrawArrays(GL_TRIANGLES, 0, modelVertices.size() / 9);
  glBindVertexArray(0);

  modelVertices.clear();

  //// 2D
  glBindVertexArray(basicVAO);

  glBindBuffer(GL_ARRAY_BUFFER, basicVBO);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(basicVertices.size() * sizeof(GLfloat)), basicVertices.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, basicEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(basicIndices.size() * sizeof(GLuint)), basicIndices.data(), GL_DYNAMIC_DRAW);

  basicShader.use();
  glDrawElements(GL_TRIANGLES, basicIndices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  
  basicVertices.clear();
  basicIndices.clear();

  window.SwapBuffers();
}

void Renderer::Begin3D(const std::shared_ptr<Camera> &camera) {
  currentCamera = camera;

  modelShader.use();
  modelShader.uniform("uViewProjection", currentCamera->GetViewProjection());
}

void Renderer::End3D() {
  currentCamera.reset();
}

auto Renderer::HasCamera() const -> bool {
  return currentCamera != nullptr;
}

void Renderer::ClearBackground(const glm::vec3 &color) {
  glClearColor(color.r, color.g, color.b, 1.f);
}

void Renderer::DrawRectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat height, const glm::vec3 &color) {
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

void Renderer::DrawCube(glm::vec3 position, GLfloat width, GLfloat height, GLfloat length, const glm::vec3 &color) {
  GLfloat x = position.x;
  GLfloat y = position.y;
  GLfloat z = position.z;

  modelVertices.insert(modelVertices.end(), {
    // position                                  // color                       // normals

    // Front face
    x - width/2, y - height/2, z + length/2,     color.r, color.g, color.b,    0.0f,  0.0f,  1.0f,   // bottom left
    x + width/2, y - height/2, z + length/2,     color.r, color.g, color.b,    0.0f,  0.0f,  1.0f,   // bottom right
    x - width/2, y + height/2, z + length/2,     color.r, color.g, color.b,    0.0f,  0.0f,  1.0f,   // top left

    x + width/2, y + height/2, z + length/2,     color.r, color.g, color.b,    0.0f,  0.0f,  1.0f,   // top right
    x - width/2, y + height/2, z + length/2,     color.r, color.g, color.b,    0.0f,  0.0f,  1.0f,   // top left
    x + width/2, y - height/2, z + length/2,     color.r, color.g, color.b,    0.0f,  0.0f,  1.0f,   // bottom right

    // Back face
    x - width/2, y - height/2, z - length/2,     color.r, color.g, color.b,    0.0f,  0.0f, -1.0f,   // bottom left
    x - width/2, y + height/2, z - length/2,     color.r, color.g, color.b,    0.0f,  0.0f, -1.0f,   // top left
    x + width/2, y - height/2, z - length/2,     color.r, color.g, color.b,    0.0f,  0.0f, -1.0f,   // bottom right

    x + width/2, y + height/2, z - length/2,     color.r, color.g, color.b,    0.0f,  0.0f, -1.0f,   // top right
    x + width/2, y - height/2, z - length/2,     color.r, color.g, color.b,    0.0f,  0.0f, -1.0f,   // bottom right
    x - width/2, y + height/2, z - length/2,     color.r, color.g, color.b,    0.0f,  0.0f, -1.0f,   // top left

    // Top face
    x - width/2, y + height/2, z - length/2,     color.r, color.g, color.b,    0.0f,  1.0f,  0.0f,   // top left
    x - width/2, y + height/2, z + length/2,     color.r, color.g, color.b,    0.0f,  1.0f,  0.0f,   // bottom left
    x + width/2, y + height/2, z + length/2,     color.r, color.g, color.b,    0.0f,  1.0f,  0.0f,   // bottom right

    x + width/2, y + height/2, z - length/2,     color.r, color.g, color.b,    0.0f,  1.0f,  0.0f,   // top right
    x - width/2, y + height/2, z - length/2,     color.r, color.g, color.b,    0.0f,  1.0f,  0.0f,   // top left
    x + width/2, y + height/2, z + length/2,     color.r, color.g, color.b,    0.0f,  1.0f,  0.0f,   // bottom right

    // Bottom face
    x - width/2, y - height/2, z - length/2,     color.r, color.g, color.b,    0.0f, -1.0f,  0.0f,   // top left
    x + width/2, y - height/2, z + length/2,     color.r, color.g, color.b,    0.0f, -1.0f,  0.0f,   // bottom right
    x - width/2, y - height/2, z + length/2,     color.r, color.g, color.b,    0.0f, -1.0f,  0.0f,   // bottom left

    x + width/2, y - height/2, z - length/2,     color.r, color.g, color.b,    0.0f, -1.0f,  0.0f,   // top right
    x + width/2, y - height/2, z + length/2,     color.r, color.g, color.b,    0.0f, -1.0f,  0.0f,   // bottom right
    x - width/2, y - height/2, z - length/2,     color.r, color.g, color.b,    0.0f, -1.0f,  0.0f,   // top left

    // Right face
    x + width/2, y - height/2, z - length/2,     color.r, color.g, color.b,    1.0f,  1.0f,  0.0f,   // bottom right
    x + width/2, y + height/2, z - length/2,     color.r, color.g, color.b,    1.0f,  1.0f,  0.0f,   // top right
    x + width/2, y + height/2, z + length/2,     color.r, color.g, color.b,    1.0f,  1.0f,  0.0f,   // top left

    x + width/2, y - height/2, z + length/2,     color.r, color.g, color.b,    1.0f,  1.0f,  0.0f,   // bottom left
    x + width/2, y - height/2, z - length/2,     color.r, color.g, color.b,    1.0f,  1.0f,  0.0f,   // bottom right
    x + width/2, y + height/2, z + length/2,     color.r, color.g, color.b,    1.0f,  1.0f,  0.0f,   // top left

    // Left face
    x - width/2, y - height/2, z - length/2,     color.r, color.g, color.b,   -1.0f,  1.0f,  0.0f,   // bottom right
    x - width/2, y + height/2, z + length/2,     color.r, color.g, color.b,   -1.0f,  1.0f,  0.0f,   // top left
    x - width/2, y + height/2, z - length/2,     color.r, color.g, color.b,   -1.0f,  1.0f,  0.0f,   // top right

    x - width/2, y - height/2, z + length/2,     color.r, color.g, color.b,   -1.0f,  1.0f,  0.0f,   // bottom left
    x - width/2, y + height/2, z + length/2,     color.r, color.g, color.b,   -1.0f,  1.0f,  0.0f,   // top left
    x - width/2, y - height/2, z - length/2,     color.r, color.g, color.b,   -1.0f,  1.0f,  0.0f    // bottom right
  });
}
