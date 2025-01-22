#ifndef BLOCK_H_
#define BLOCK_H_

namespace World {

typedef enum BlockType {
  AIR = 0,
  GRASS,
  DIRT,
  STONE
} BlockType;

class Block {
public:
  Block() = default;
  Block(BlockType type);

  ~Block() = default;

  bool IsSolid() const;
  BlockType GetType() const;
  void SetType(BlockType value);

private:
  BlockType type = BlockType::AIR;
};

}

#endif // BLOCK_H_
