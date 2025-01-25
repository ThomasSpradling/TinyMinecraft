#include "World/BlockAtlas.h"
#include "Utils/Profiler.h"
#include "Utils/defs.h"
#include "World/BlockFace.h"

namespace World {

constexpr glm::vec2 BlockAtlas::tileSize;

glm::vec2 BlockAtlas::GetNormalizedTextureCoords(BlockType type, BlockFace face) {
  glm::ivec2 pixelCoords = GetTextureCoords(type, face);

  float normalizedX = static_cast<float>(pixelCoords.x) / BLOCK_TEXTURE_ATLAS_WIDTH;
  float normalizedY = static_cast<float>(pixelCoords.y) / BLOCK_TEXTURE_ATLAS_HEIGHT;

  return { normalizedX, normalizedY };
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
    case BlockType::SAND:
      coords = { 4, 0 };
      break;
    case BlockType::LOG:
      if (face == BlockFace::Top || face == BlockFace::Bottom)
        coords = { 0, 1 };
      else coords = { 1, 1 };
      break;
    case BlockType::SNOW:
      coords = { 2, 1 };
      break;
    case BlockType::WATER:
      coords = { 2, 1 };
      break;
    case BlockType::SANDSTONE:
      coords = { 4, 1 };
      break;
    case BlockType::AIR:
    default:
      return glm::ivec2(0);
  }

  return coords * BLOCK_TEXTURE_WIDTH;
}

}
