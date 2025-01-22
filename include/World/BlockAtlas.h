#ifndef BLOCK_ATLAS_H_
#define BLOCK_ATLAS_H_

#include "Utils/defs.h"
#include "World/Block.h"
#include "World/BlockFace.h"

namespace World {

class BlockAtlas {
public:

  static constexpr glm::vec2 tileSize = glm::vec2(
    static_cast<float>(BLOCK_TEXTURE_WIDTH) / BLOCK_TEXTURE_ATLAS_WIDTH,
    static_cast<float>(BLOCK_TEXTURE_WIDTH) / BLOCK_TEXTURE_ATLAS_HEIGHT
  );

  BlockAtlas() = delete;
  ~BlockAtlas() = delete;

  static glm::vec2 GetNormalizedTextureCoords(BlockType type, BlockFace face);


private:
  static glm::ivec2 GetTextureCoords(BlockType type, BlockFace face);
};

}

#endif // BLOCK_ATLAS_H_
