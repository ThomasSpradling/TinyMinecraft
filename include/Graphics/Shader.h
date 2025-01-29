#ifndef SHADER_H_
#define SHADER_H_

#include "Graphics/gfx.h"
#include "Utils/mathgl.h"
#include <string>
#include <string_view>

namespace Graphics {

struct VertexAttr {
    GLuint loc;
    std::string name;
};

class Shader {
public:
  Shader(const std::string &vertexPath, const std::string &fragmentPath);

  ~Shader();
  Shader(const Shader &) = delete;
  auto operator=(const Shader &) -> Shader & = delete;
  Shader(Shader &&) = delete;
  auto operator=(Shader &&) -> Shader & = delete;

  auto Use() -> void;

  template<typename T> void Uniform(const std::string &name, T value);
  
  void Uniform(const std::string &name, bool value) {
    Use();
    glUniform1i(glGetUniformLocation(m_programHandle, name.c_str()), (int) value);
  }

  auto Uniform(const std::string &name, int value) -> void {
    Use();
    glUniform1i(glGetUniformLocation(m_programHandle, name.c_str()), value);
  }

  auto Uniform(const std::string &name, float value) -> void {
    Use();
    glUniform1f(glGetUniformLocation(m_programHandle, name.c_str()), value);
  }

  auto Uniform(const std::string &name, glm::mat4 value) -> void {
    Use();
    glUniformMatrix4fv(glGetUniformLocation(m_programHandle, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
  }

  auto Uniform(const std::string &name, glm::mat3 value) -> void {
    Use();
    glUniformMatrix3fv(glGetUniformLocation(m_programHandle, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
  }

  auto Uniform(const std::string &name, glm::vec2 value) -> void {
    Use();
    glUniform2fv(glGetUniformLocation(m_programHandle, name.c_str()), 1, glm::value_ptr(value));
  }

  auto Uniform(const std::string &name, glm::vec3 value) -> void {
    Use();
    glUniform3fv(glGetUniformLocation(m_programHandle, name.c_str()), 1, glm::value_ptr(value));
  }

  auto Uniform(const std::string &name, glm::vec4 value) -> void {
    Use();
    glUniform4fv(glGetUniformLocation(m_programHandle, name.c_str()), 1, glm::value_ptr(value));
  }
private:
  GLuint m_programHandle;
  GLuint m_vertexHandle, m_fragmentHandle;

  [[nodiscard]] auto ReadFile(const std::string &path) const -> std::string;
  auto Compile(const char *source, GLenum type, const std::string_view filename) const -> GLuint;
  [[nodiscard]] auto Link() const -> GLuint;
};

}

#endif // SHADER_H