#include "Utils/defs.h"
#include "Utils/mathgl.h"
#include "Graphics/gfx.h"
#include <vector>

namespace UI {

class Text {
public:
  static constexpr glm::vec2 tileSize = glm::vec2(
    static_cast<float>(TEXT_CHAR_WIDTH) / static_cast<float>(FONT_MAP_WIDTH),
    static_cast<float>(TEXT_CHAR_HEIGHT) / static_cast<float>(FONT_MAP_HEIGHT)
  );

  Text() = default;

  
  void Initialize();
  void Arrange();

  void Update();
  GLuint GetVAO();
  GLsizei GetVertexCount();

  void Draw(const std::string &text, GLfloat x, GLfloat y, const glm::vec3 &color);
private:
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;

  GLuint VAO, VBO, EBO;

};

}
