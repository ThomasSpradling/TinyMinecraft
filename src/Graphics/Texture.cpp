#include "Graphics/Texture.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace Graphics {

void Texture::Load(const std::string &filePath) {
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

  Bind();

  glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);

  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


  // glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::Bind() {
  glBindTexture(GL_TEXTURE_2D, handle);
}

}
