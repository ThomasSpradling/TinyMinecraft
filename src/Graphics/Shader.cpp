#include "Graphics/Shader.h"
#include "Graphics/gfx.h"
#include "Utils/Logger.h"
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>

#define LOG_READ_SIZE 512

namespace TinyMinecraft {

  namespace Graphics {

    Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
      std::string rawVertexFile = ReadFile(vertexPath);
      std::string rawFragmentFile = ReadFile(fragmentPath);

      m_vertexHandle = Compile(rawVertexFile.c_str(), GL_VERTEX_SHADER, vertexPath);
      m_fragmentHandle = Compile(rawFragmentFile.c_str(), GL_FRAGMENT_SHADER, fragmentPath);

      m_programHandle = Link();
    }

    Shader::~Shader() {
      glDeleteProgram(m_programHandle);
    }

    auto Shader::Use() -> void {
      glUseProgram(m_programHandle);
    }

    auto Shader::ReadFile(const std::string &path) const -> std::string {
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
        Utils::g_logger.Error("Shader: Failed to read file {}.", path);
        exit(1);
      }
      return code;
    }

    auto Shader::Compile(const char *source, GLenum type, const std::string_view filename) const -> GLuint {
      assert(source != nullptr);
      assert(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER);

      GLuint shader = glCreateShader(type);

      glShaderSource(shader, 1, &source, nullptr);
      glCompileShader(shader);


      std::array<char, LOG_READ_SIZE> infoLog {};
      int success = false;
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success) {
        glGetShaderInfoLog(shader, LOG_READ_SIZE, nullptr, infoLog.data());
        // Utils::g_logger.Error("Shader: Failed to compile shader at {}. Error Message:\n{}", filename, infoLog.data());
      }

      return shader;
    }

    auto Shader::Link() const -> GLuint {
      GLuint program = glCreateProgram();
      glAttachShader(program, m_vertexHandle);
      glAttachShader(program, m_fragmentHandle);
      glLinkProgram(program);

      std::array<char, LOG_READ_SIZE> infoLog {};
      int success = false;
      glGetProgramiv(m_programHandle, GL_LINK_STATUS, &success);
      if (!success) {
        glGetShaderInfoLog(m_programHandle, LOG_READ_SIZE, nullptr, infoLog.data());
        // Utils::g_logger.Error("Shader: Failed to link shaders. Error Message:\n{}", infoLog, infoLog.data());
      }

      glDeleteShader(m_vertexHandle);
      glDeleteShader(m_fragmentHandle);

      return program;
    }

  }

}
