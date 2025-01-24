#include "Graphics/Texture.h"
#include "Graphics/gfx.h"
#include <iostream>

#pragma clang diagnostic push 
#pragma clang diagnostic ignored "-Wcast-qual"
#pragma clang diagnostic ignored "-Wcast-align"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wsign-conversion"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#pragma clang diagnostic pop

namespace Graphics {

void Texture::Load(const std::string &filePath, int id) {
  glGenTextures(1, &handle);

  unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &channelCount, 0);
  if (data == nullptr) {
    std::cerr << "Error loading image at " << filePath << std::endl;
    exit(1);
  }

  GLenum format;
  if (channelCount == 1)
    format = GL_RGB;
  else if (channelCount == 3)
    format = GL_RGB;
  else if (channelCount == 4)
    format = GL_RGBA;
  else {
    std::cerr << "Invalid number of channels!" << std::endl;
    exit(1);
  }

  Bind(id);

  glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);
  glGenerateMipmap(GL_TEXTURE_2D);
  
  float maxAnisotropy;
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
  glTexParameterf(GL_TEXTURE_2D, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

  glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::Bind(int id) {

  if (id == 0) {
    glActiveTexture(GL_TEXTURE0);
  }

  if (id == 1) {
    glActiveTexture(GL_TEXTURE1);
  }
  glBindTexture(GL_TEXTURE_2D, handle);
}

}
