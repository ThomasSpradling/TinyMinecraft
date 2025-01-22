#include "World/Block.h"

namespace World {

Block::Block(BlockType type) : type(type) {}

bool Block::IsSolid() const {
  return type != BlockType::AIR;
}

BlockType Block::GetType() const {
  return type;
}

void Block::SetType(BlockType value) {
  type = value;
}

}
