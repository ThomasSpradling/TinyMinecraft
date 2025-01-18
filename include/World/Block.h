#ifndef BLOCK_H_
#define BLOCK_H_

namespace World {

typedef enum BlockType {
  AIR = 0,
  DIRT,
} BlockType;

class Block {
public:
  Block() = default;
  Block(BlockType type);

  ~Block() = default;

  bool IsSolid() const;
  BlockType GetType() const;

  // glm::vec2 GetTextureCoords(Face face) const;
private:
  BlockType type = BlockType::AIR;
};

}

#endif // BLOCK_H_
