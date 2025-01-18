#include "World/Block.h"

namespace World {

Block::Block(BlockType type) : type(type) {}

bool Block::IsSolid() const {
  return type != BlockType::AIR;
}

BlockType Block::GetType() const {
  return type;
}

// glm::vec2 Block::GetTextureCoords(Face face) const {
//   if (!IsSolid()) {
//     return glm::vec2(0.f);
//   }

//   const int spriteWidth = 32;
//   glm::vec2 textureCoord;

//   switch (type) {
//   case BlockType::DIRT:
//     return glm::vec2(0, 0);
//   default:
//     return glm::vec2(0.f);
//   }
// }

}
