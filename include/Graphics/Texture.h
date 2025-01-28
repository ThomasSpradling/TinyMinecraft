#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Graphics/gfx.h"
#include <string>

namespace Graphics {

class Texture {
public:
  Texture(const std::string &filePath);

  ~Texture();
  Texture(const Texture &) = delete;
  auto operator=(const Texture &) -> Texture & = delete;
  Texture(const Texture &&) = delete;
  auto operator=(const Texture &&) -> Texture & = delete;

  void Bind() const;
  [[nodiscard]] auto GetId() const -> GLuint;

private:
  static int currentTextureId;

  GLuint m_handle;
  GLuint m_id;
  int m_width = 0;
  int m_height = 0;
  int m_channelCount = 0;
};

}

#endif // TEXTURE_H_
