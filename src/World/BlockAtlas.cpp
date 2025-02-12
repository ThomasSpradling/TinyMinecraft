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
        case BlockType::Grass:
          if (face == Geometry::Face::Top)
            coords = { 0, 0 };
          else if (face == Geometry::Face::Bottom)
            coords = { 2, 0 };
          else coords = { 1, 0 };
          break;
        case BlockType::Dirt:
          coords = { 2, 0 };
          break;
        case BlockType::Stone:
          coords = { 3, 0 };
          break;
        case BlockType::Sand:
          coords = { 4, 0 };
          break;
        case BlockType::Log:
          if (face == Geometry::Face::Top || face == Geometry::Face::Bottom)
            coords = { 0, 1 };
          else coords = { 1, 1 };
          break;
        case BlockType::Snow:
          coords = { 2, 1 };
          break;
        case BlockType::Water:
          coords = { 3, 1 };
          break;
        case BlockType::Glass:
          coords = { 4, 1 };
          break;
        case BlockType::Leaves:
          coords = { 0, 2 };
          break;
        case BlockType::TallGrass:
          coords = { 1, 2 };
          break;
        case BlockType::Air:
        default:
          return glm::ivec2(0);
      }

      return coords * BLOCK_TEXTURE_WIDTH;
    }

  }

}
