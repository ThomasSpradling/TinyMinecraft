#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Graphics/gfx.h"
#include <string>

namespace Graphics {

class Texture {
public:
  Texture() = default;
  ~Texture() = default;

  void Load(const std::string &filePath);
  void Bind();

private:
  GLuint handle;
  int width;
  int height;
  int channelCount;
};

}

#endif // TEXTURE_H_
