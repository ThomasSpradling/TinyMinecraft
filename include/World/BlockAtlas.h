#ifndef BLOCK_ATLAS_H_
#define BLOCK_ATLAS_H_

#include "Utils/defs.h"
#include "World/Block.h"
#include "World/BlockFace.h"

namespace World {

class BlockAtlas {
public:
  BlockAtlas() = delete;
  ~BlockAtlas() = delete;

  static glm::vec2 GetNormalizedTextureCoords(BlockType type, BlockFace face);

  static glm::vec2 GetTextureSize();

private:
  static glm::ivec2 GetTextureCoords(BlockType type, BlockFace face);
};

}

#endif // BLOCK_ATLAS_H_
