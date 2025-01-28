#include "World/Block.h"
#include "glm/fwd.hpp"

namespace World {

Block::Block(BlockType type) : m_type(type) {}

bool Block::IsTransparent() const {
  switch (m_type) {
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
  switch (m_type) {
    case World::BlockType::TALL_GRASS:
      return true;
    default:
      return false;
  }
}

BlockType Block::GetType() const {
  return m_type;
}

void Block::SetType(BlockType value) {
  m_type = value;
}

}
