#ifndef BLOCK_ATLAS_H_
#define BLOCK_ATLAS_H_

#include "Geometry/geometry.h"
#include "Utils/Singleton.h"
#include "Utils/defs.h"
#include "World/Block.h"
#include "World/BlockType.h"

namespace TinyMinecraft {

  namespace World {

    class BlockAtlas : Utils::Singleton {
    public:

      static constexpr glm::vec2 tileSize = glm::vec2(
        static_cast<float>(BLOCK_TEXTURE_WIDTH) / BLOCK_TEXTURE_ATLAS_WIDTH,
        static_cast<float>(BLOCK_TEXTURE_WIDTH) / BLOCK_TEXTURE_ATLAS_HEIGHT
      );

      static auto GetNormalizedTextureCoords(BlockType type, Geometry::Face face) -> glm::vec2;

    private:
      static auto GetTextureCoords(BlockType type, Geometry::Face face) -> glm::ivec2;
    };

  }
}

#endif // BLOCK_ATLAS_H_
