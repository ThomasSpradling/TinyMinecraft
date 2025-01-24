#ifndef USER_INTERFACE_H_
#define USER_INTERFACE_H_

#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
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

  void Render(Graphics::Shader &uiShader, Graphics::Shader &textShader, Graphics::Texture &fontMap);
  void Update();
  GLuint GetVAO();
  GLsizei GetVertexCount();
private:
  std::vector<GLfloat> basicVertices, textVertices;
  std::vector<GLuint> basicIndices, textIndices;

  GLuint basicVAO, basicVBO, basicEBO;
  GLuint textVAO, textVBO, textEBO;

  int basicVertexCount, textVertexCount;

  glm::mat4 projection;

  void InitializeGeneral();
  void InitializeText();
  void UpdateHelper(GLuint VAO, GLuint VBO, GLuint EBO, std::vector<GLfloat> &verts, std::vector<GLuint> &indices);

  void DrawRectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat height, const glm::vec3 &color);
  void DrawText(const std::string &text, GLfloat x, GLfloat y, const glm::vec3 &color);

};

}

#endif // USER_INTERFACE_H_
