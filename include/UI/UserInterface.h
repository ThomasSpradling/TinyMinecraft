#ifndef USER_INTERFACE_H_
#define USER_INTERFACE_H_

#include "Graphics/gfx.h"
#include "Utils/mathgl.h"
#include <vector>

namespace UI {

class UserInterface {
public:
  UserInterface() = default;
  ~UserInterface() = default;

  void Initialize();
  void Arrange();

  void Update();
  GLuint GetVAO();
  GLsizei GetVertexCount();
private:
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;

  GLuint VAO, VBO, EBO;

  void DrawRectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat height, const glm::vec3 &color);
};

}

#endif // USER_INTERFACE_H_
