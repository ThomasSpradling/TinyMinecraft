#ifndef BLOCK_H_
#define BLOCK_H_

#include "Utils/mathgl.h"

namespace World {

typedef enum BlockType {
  AIR = 0,
  GRASS = 1,
  DIRT = 2,
  STONE = 3,
  SAND = 4,
  LOG = 5,
  SNOW = 6,
  WATER = 7,
  SANDSTONE = 8,
  GLASS = 9,
  LEAVES = 10,
  TALL_GRASS
} BlockType;

class Block {
public:
  Block() = default;
  Block(BlockType type);

  ~Block() = default;

  bool IsTransparent() const;
  bool IsBillboard() const;
  bool IsFluid() const;

  BlockType GetType() const;
  void SetType(BlockType value);

private:
  BlockType m_type = BlockType::AIR;
};

}

#endif // BLOCK_H_
