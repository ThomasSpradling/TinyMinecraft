#include "World/BlockAtlas.h"
#include "Utils/Profiler.h"
#include "Utils/defs.h"
#include "World/Block.h"
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
      if (face == BlockFace::Up)
        coords = { 0, 0 };
      else if (face == BlockFace::Down)
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
      if (face == BlockFace::Up || face == BlockFace::Down)
        coords = { 0, 1 };
      else coords = { 1, 1 };
      break;
    case BlockType::SNOW:
      coords = { 2, 1 };
      break;
    case BlockType::WATER:
      coords = { 3, 1 };
      break;
    case BlockType::GLASS:
      coords = { 4, 1 };
      break;
    case BlockType::LEAVES:
      coords = { 0, 2 };
      break;
    case BlockType::TALL_GRASS:
      coords = { 1, 2 };
      break;
    case BlockType::AIR:
    default:
      return glm::ivec2(0);
  }

  return coords * BLOCK_TEXTURE_WIDTH;
}

}
