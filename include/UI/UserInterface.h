#ifndef USER_INTERFACE_H_
#define USER_INTERFACE_H_

#include "Graphics/BufferObject.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/VertexArray.h"
#include "Graphics/gfx.h"
#include "Utils/mathgl.h"
#include "World/Biome.h"
#include <vector>

namespace UI {

class UserInterface {
public:
  UserInterface();

  void Arrange();

  void Render(Graphics::Shader &uiShader, Graphics::Shader &textShader, Graphics::Texture &fontMap);
  void Update();
  
  void SetCurrentFPS(int fps);
  void SetPlayerPosition(const glm::vec3 &pos);
  void SetChunkPosition(const glm::ivec2 &pos);
  void SetClimateValues(float temperature, float humidity, World::BiomeType biome);
private:
  std::vector<GLfloat> m_basicVertices, m_textVertices;
  std::vector<GLuint> m_basicIndices, m_textIndices;

  Graphics::VertexArray m_basicVAO, m_textVAO;
  Graphics::BufferObject m_basicVBO, m_textVBO, m_basicEBO, m_textEBO;

  size_t m_basicVertexCount = 0, m_textVertexCount = 0;

  // debug
  int m_currentFPS = 0;
  glm::vec3 m_playerPosition { 0.f };
  glm::ivec2 m_chunkPosition { 0 };

  float m_temperature = 0, m_humidity = 0;
  World::BiomeType m_biome { World::BiomeType::Grassland };

  void InitializeGeneral();
  void InitializeText();
  void UpdateHelper(Graphics::VertexArray &vao, Graphics::BufferObject &vbo, Graphics::BufferObject &ebo, std::vector<GLfloat> &verts, std::vector<GLuint> &indices);

  void DrawRectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat height, const glm::vec3 &color);
  void DrawText(const std::string &text, GLfloat x, GLfloat y, const glm::vec3 &color, float scale);
};

}

#endif // USER_INTERFACE_H_
