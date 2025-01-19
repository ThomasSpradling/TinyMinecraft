#include "World/BlockAtlas.h"
#include "Utils/defs.h"
#include <iostream>

namespace World {

glm::vec2 BlockAtlas::GetNormalizedTextureCoords(BlockType type, BlockFace face) {
  glm::ivec2 pixelCoords = GetTextureCoords(type, face);

  float normalizedX = static_cast<float>(pixelCoords.x) / BLOCK_TEXTURE_ATLAS_WIDTH;
  float normalizedY = static_cast<float>(pixelCoords.y) / BLOCK_TEXTURE_ATLAS_HEIGHT;

  return { normalizedX, normalizedY };
}

glm::vec2 BlockAtlas::GetTextureSize() {
  return {
    static_cast<float>(BLOCK_TEXTURE_WIDTH) / BLOCK_TEXTURE_ATLAS_WIDTH,
    static_cast<float>(BLOCK_TEXTURE_WIDTH) / BLOCK_TEXTURE_ATLAS_HEIGHT
  };
}

glm::ivec2 BlockAtlas::GetTextureCoords(BlockType type, BlockFace face) {
  glm::ivec2 coords;

  switch(type) {
    case BlockType::GRASS:
      if (face == BlockFace::Top)
        coords = { 0, 0 };
      else if (face == BlockFace::Bottom)
        coords = { 2, 0 };
      else coords = { 1, 0 };
      break;
    case BlockType::DIRT:
      coords = { 2, 0 };
      break;
    case BlockType::STONE:
      coords = { 3, 0 };
      break;
    case BlockType::AIR:
    default:
      return glm::ivec2(0);
  }

  return coords * BLOCK_TEXTURE_WIDTH;
}

}
