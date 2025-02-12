#ifndef BLOCK_H_
#define BLOCK_H_

#include "Utils/mathgl.h"
#include "World/BlockType.h"

namespace TinyMinecraft {

  namespace World {

    enum BlockRenderType {
      Empty,
      Fluid,
      Standard,
      Foliage
    };

    struct BlockDefinition {
      BlockType type;
      std::string name;
      int id;
      BlockRenderType renderType;
      bool isSolid;
      bool isTranslucent;
    };

    class BlockData {
    public:
      static void Initialize();
      [[nodiscard]] static inline auto Get(BlockType block) -> BlockDefinition { return data[block]; }
      [[nodiscard]] static inline auto IsSolid(BlockType block) -> bool { return data[block].isSolid; }
      [[nodiscard]] static inline auto IsEmpty(BlockType block) -> bool { return data[block].renderType == BlockRenderType::Empty; }
      [[nodiscard]] static inline auto IsTranslucent(BlockType block) -> bool { return data[block].isTranslucent; }
    private:
      static std::unordered_map<BlockType, BlockDefinition> data;
      
      static void LoadBlockData(BlockType block, const std::string &path);
    };
    
  }
  
  auto operator<<(std::ostream &os, BlockType type) -> std::ostream &;

}

#endif // BLOCK_H_
