#ifndef SHADER_H_
#define SHADER_H_

#include "gfx.h"
#include "utils/math.h"
#include <string>
#include <string_view>

struct VertexAttr {
    GLuint loc;
    std::string name;
};

class Shader {
public:
    GLuint handle;

    Shader();
    ~Shader();

    void Init(const std::string_view vs_path, const std::string_view fs_path);

    auto use() -> void;
    template<typename T> auto uniform(std::string name, T value) -> void;
    template<> auto uniform<bool>(std::string name, bool value) -> void;
    template<> auto uniform<int>(std::string name, int value) -> void;
    template<> auto uniform<float>(std::string name, float value) -> void;
    template<> auto uniform<glm::mat4>(std::string name, glm::mat4 value) -> void;
    template<> auto uniform<glm::mat3>(std::string name, glm::mat3 value) -> void;
    template<> auto uniform<glm::vec2>(std::string name, glm::vec2 value) -> void;
    template<> auto uniform<glm::vec3>(std::string name, glm::vec3 value) -> void;
    template<> auto uniform<glm::vec4>(std::string name, glm::vec4 value) -> void;
private:
    GLuint vs_handle, fs_handle;

    auto read_file(const std::string_view path) const -> std::string;
    auto compile(const char *source, GLenum type, const std::string_view filename) const -> GLuint;
    auto link() const -> GLuint;
};

#endif // SHADER_H