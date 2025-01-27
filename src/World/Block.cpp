#include "World/Block.h"
#include "glm/fwd.hpp"

namespace World {

Block::Block(BlockType type) : type(type) {}

bool Block::IsTransparent() const {
  switch (type) {
    case World::BlockType::AIR:
    case World::BlockType::WATER:
    case World::BlockType::GLASS:
    case World::BlockType::LEAVES:
    case World::BlockType::TALL_GRASS:
      return true;
    default:
      return false;
  }
}

bool Block::IsBillboard() const {
  switch (type) {
    case World::BlockType::TALL_GRASS:
      return true;
    default:
      return false;
  }
}

BlockType Block::GetType() const {
  return type;
}

void Block::SetType(BlockType value) {
  type = value;
}

}
