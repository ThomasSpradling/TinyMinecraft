#include "Graphics/Texture.h"
#include "Graphics/gfx.h"
#include "Utils/Logger.h"
#include <iostream>

#pragma clang diagnostic push 
#pragma clang diagnostic ignored "-Wcast-qual"
#pragma clang diagnostic ignored "-Wcast-align"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wsign-conversion"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#pragma clang diagnostic pop

#define SET_ACTIVE(i) glActiveTexture(GL_TEXTURE##i);
#define MAX_MIP_MAPS 5

namespace Graphics {

int Texture::currentTextureId { 0 };

// NOLINTBEGIN(cppcoreguidelines-pro-type-member-init)
Texture::Texture(const std::string &filePath)
  : m_id(Texture::currentTextureId++)
{
  glGenTextures(1, &m_handle);

  unsigned char *data = stbi_load(filePath.c_str(), &m_width, &m_height, &m_channelCount, 0);
  if (data == nullptr) {
    Utils::g_logger.Error("Texture {}: Error loading image at {}.", m_id, filePath);
    exit(1);
  }

  GLenum format = GL_RGB;
  if (m_channelCount == 1)
    format = GL_R;
  else if (m_channelCount == 3)
    format = GL_RGB;
  else if (m_channelCount == 4)
    format = GL_RGBA;
  else {
    Utils::g_logger.Error("Texture {}: Invalid number of channels!", m_id);
    exit(1);
  }

  Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
  stbi_image_free(data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, MAX_MIP_MAPS);
  glGenerateMipmap(GL_TEXTURE_2D);
  
  float maxAnisotropy = 0.f;
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
  glTexParameterf(GL_TEXTURE_2D, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

  glGenerateMipmap(GL_TEXTURE_2D);
}
// NOLINTEND(cppcoreguidelines-pro-type-member-init)

Texture::~Texture() {
  glDeleteTextures(1, &m_id);
}

void Texture::Bind() const {
  if (m_id == 0) SET_ACTIVE(0)
  if (m_id == 1) SET_ACTIVE(1)
  if (m_id == 2) SET_ACTIVE(2)
  if (m_id == 3) SET_ACTIVE(3)
  if (m_id == 4) SET_ACTIVE(4)
  if (m_id == 5) SET_ACTIVE(5)
  if (m_id == 6) SET_ACTIVE(6)
  if (m_id == 7) SET_ACTIVE(7)
  if (m_id == 8) SET_ACTIVE(8)
  if (m_id == 9) SET_ACTIVE(9)

  glBindTexture(GL_TEXTURE_2D, m_handle);
}

auto Texture::GetId() const -> GLuint {
  return m_id;
}

}
