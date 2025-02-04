#include "World/BlockAtlas.h"
#include "Utils/defs.h"
#include "World/Block.h"

namespace TinyMinecraft {

  namespace World {

    constexpr glm::vec2 BlockAtlas::tileSize;

    auto BlockAtlas::GetNormalizedTextureCoords(BlockType type, Geometry::Face face) -> glm::vec2 {
      glm::ivec2 pixelCoords = GetTextureCoords(type, face);

      float normalizedX = static_cast<float>(pixelCoords.x) / BLOCK_TEXTURE_ATLAS_WIDTH;
      float normalizedY = static_cast<float>(pixelCoords.y) / BLOCK_TEXTURE_ATLAS_HEIGHT;

      return { normalizedX, normalizedY };
    }

    auto BlockAtlas::GetTextureCoords(BlockType type, Geometry::Face face) -> glm::ivec2 {
      glm::ivec2 coords;

      switch(type) {
        case BlockType::GRASS:
          if (face == Geometry::Face::Up)
            coords = { 0, 0 };
          else if (face == Geometry::Face::Down)
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
          if (face == Geometry::Face::Up || face == Geometry::Face::Down)
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

}
