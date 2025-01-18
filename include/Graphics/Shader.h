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
    GLuint handle;

    Shader() = default;
    ~Shader();

    void Load(const std::string_view vs_path, const std::string_view fs_path);

    auto Use() -> void;
    template<typename T> auto Uniform(std::string name, T value) -> void;
    template<> auto Uniform<bool>(std::string name, bool value) -> void;
    template<> auto Uniform<int>(std::string name, int value) -> void;
    template<> auto Uniform<float>(std::string name, float value) -> void;
    template<> auto Uniform<glm::mat4>(std::string name, glm::mat4 value) -> void;
    template<> auto Uniform<glm::mat3>(std::string name, glm::mat3 value) -> void;
    template<> auto Uniform<glm::vec2>(std::string name, glm::vec2 value) -> void;
    template<> auto Uniform<glm::vec3>(std::string name, glm::vec3 value) -> void;
    template<> auto Uniform<glm::vec4>(std::string name, glm::vec4 value) -> void;
private:
    GLuint vs_handle, fs_handle;

    auto ReadFile(const std::string_view path) const -> std::string;
    auto Compile(const char *source, GLenum type, const std::string_view filename) const -> GLuint;
    auto Link() const -> GLuint;
};

}

#endif // SHADER_H