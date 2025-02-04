#ifndef BLOCK_ATLAS_H_
#define BLOCK_ATLAS_H_

#include "Geometry/geometry.h"
#include "Utils/defs.h"
#include "World/Block.h"

namespace TinyMinecraft {

  namespace World {

    class BlockAtlas {
    public:

      static constexpr glm::vec2 tileSize = glm::vec2(
        static_cast<float>(BLOCK_TEXTURE_WIDTH) / BLOCK_TEXTURE_ATLAS_WIDTH,
        static_cast<float>(BLOCK_TEXTURE_WIDTH) / BLOCK_TEXTURE_ATLAS_HEIGHT
      );

      BlockAtlas() = delete;
      ~BlockAtlas() = delete;
      BlockAtlas(const BlockAtlas &) = delete;
      auto operator=(const BlockAtlas &) -> BlockAtlas & = delete;
      BlockAtlas(BlockAtlas &&) = delete;
      auto operator=(BlockAtlas &&) -> BlockAtlas & = delete;

      static auto GetNormalizedTextureCoords(BlockType type, Geometry::Face face) -> glm::vec2;


    private:
      static auto GetTextureCoords(BlockType type, Geometry::Face face) -> glm::ivec2;
    };

  }
}

#endif // BLOCK_ATLAS_H_
