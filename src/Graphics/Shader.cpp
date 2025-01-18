#include "Graphics/Shader.h"
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>

namespace Graphics {

Shader::~Shader() {
    glDeleteShader(handle);
}

void Shader::Load(const std::string_view vs_path, const std::string_view fs_path) {
  std::string vertex_code = ReadFile(vs_path);
  std::string fragment_code = ReadFile(fs_path);

  const char *vs_code = vertex_code.c_str();
  const char *fs_code = fragment_code.c_str();

  vs_handle = Compile(vs_code, GL_VERTEX_SHADER, vs_path);
  fs_handle = Compile(fs_code, GL_FRAGMENT_SHADER, fs_path);

  handle = Link();
}


auto Shader::Use() -> void {
    glUseProgram(handle);
}

template<>
auto Shader::Uniform<bool>(std::string name, bool value) -> void {
    glUniform1i(glGetUniformLocation(handle, name.c_str()), (int) value);
}

template<>
auto Shader::Uniform<int>(std::string name, int value) -> void {
    glUniform1i(glGetUniformLocation(handle, name.c_str()), value);
}

template<>
auto Shader::Uniform<float>(std::string name, float value) -> void {
    glUniform1f(glGetUniformLocation(handle, name.c_str()), value);
}

template<>
auto Shader::Uniform<glm::mat4>(std::string name, glm::mat4 value) -> void {
    glUniformMatrix4fv(glGetUniformLocation(handle, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

template<>
auto Shader::Uniform<glm::mat3>(std::string name, glm::mat3 value) -> void {
    glUniformMatrix3fv(glGetUniformLocation(handle, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

template<>
auto Shader::Uniform<glm::vec2>(std::string name, glm::vec2 value) -> void {
    glUniform2fv(glGetUniformLocation(handle, name.c_str()), 1, glm::value_ptr(value));
}

template<>
auto Shader::Uniform<glm::vec3>(std::string name, glm::vec3 value) -> void {
    glUniform3fv(glGetUniformLocation(handle, name.c_str()), 1, glm::value_ptr(value));
}

template<>
auto Shader::Uniform<glm::vec4>(std::string name, glm::vec4 value) -> void {
    glUniform4fv(glGetUniformLocation(handle, name.c_str()), 1, glm::value_ptr(value));
}

//// PRIVATE METHODS ////

auto Shader::ReadFile(const std::string_view path) const -> std::string {
    std::string code;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        file.open(path);

        std::stringstream stream;
        stream << file.rdbuf();

        file.close();

        code = stream.str();
    } catch (std::ifstream::failure e) {
        std::cerr << "Failed to read file: " << path << std::endl;
    }
    return code;
}

auto Shader::Compile(const char *source, GLenum type, const std::string_view filename) const -> GLuint {
    assert(source != nullptr);
    assert(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER);

    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    char info_log[512];
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        std::cout << "Failed to compile shader at path " << filename << ".\n" << info_log << std::endl;
    }

    return shader;
}

auto Shader::Link() const -> GLuint {
    GLuint program = glCreateProgram();
    glAttachShader(program, vs_handle);
    glAttachShader(program, fs_handle);
    glLinkProgram(program);

    char info_log[512];
    int success;
    glGetProgramiv(handle, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(handle, 512, NULL, info_log);
        std::cout << "Failed to link shaders at path.\n" << info_log << std::endl;
    }

    glDeleteShader(vs_handle);
    glDeleteShader(fs_handle);

    return program;
}

}
