#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Graphics/gfx.h"
#include "Utils/NonCopyable.h"
#include "Utils/NonMovable.h"
#include <string>

namespace TinyMinecraft {

  namespace Graphics {

    class Texture : private Utils::NonCopyable, private Utils::NonMoveable {
    public:
      Texture(const std::string &filePath);

      ~Texture();

      void Bind() const;
      [[nodiscard]] inline auto GetId() const -> GLuint { return m_id; };

    private:
      static int currentTextureId;

      GLuint m_handle;
      GLuint m_id;
      int m_width = 0;
      int m_height = 0;
      int m_channelCount = 0;
    };

  }

}

#endif // TEXTURE_H_
