#ifndef RENDERER_H_
#define RENDERER_H_

#include "Camera.h"
#include "Shader.h"
#include "Window.h"
#include "gfx.h"
#include "utils/math.h"
#include <memory>
#include <vector>

class Renderer {
public:
  Renderer(Window &window);
  ~Renderer();

  void Init();
  
  void StartDrawing();
  void StopDrawing();

  void Begin3D(const std::shared_ptr<Camera> &camera);
  void End3D();
  auto HasCamera() const -> bool;
  void RefreshCamera();

  void ClearBackground(const glm::vec3 &color);

  void DrawRectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat height, const glm::vec3 &color);
  void DrawCube(glm::vec3 position, GLfloat width, GLfloat height, GLfloat length, const glm::vec3 &color);

private:
  Window &window;

  Shader basicShader;
  GLuint basicVAO, basicVBO, basicEBO;
  std::vector<GLfloat> basicVertices;
  std::vector<GLuint> basicIndices;

  Shader modelShader;
  GLuint modelVAO, modelVBO;
  std::vector<GLfloat> modelVertices;

  std::shared_ptr<Camera> currentCamera = nullptr;
};

#endif // RENDERER_H_