#ifndef RENDERER_H_
#define RENDERER_H_

#include "Shader.h"
#include "Window.h"
#include "gfx.h"
#include "utils/math.h"
#include <vector>

class Renderer {
public:
  Renderer(Window &window);
  ~Renderer();

  void Init();
  
  void StartDrawing();
  void StopDrawing();

  void DrawRectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat height, const glm::vec3 &color);
  void DrawCube(glm::vec3 &position, GLfloat width, GLfloat height, GLfloat length, const glm::vec3 &color);

  void ClearBackground(const glm::vec3 &color);
private:
  Window &window;
  Shader shader;
  GLuint VAO, VBO, EBO;

  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;
};

#endif // RENDERER_H_