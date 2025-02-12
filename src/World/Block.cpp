#include "World/Block.h"

#include "Utils/Logger.h"
#include "World/BlockType.h"
#include "nlohmann/json.hpp"
#include <fstream>

namespace TinyMinecraft {

  namespace World {

    std::unordered_map<BlockType, BlockDefinition> BlockData::data;

    void BlockData::Initialize() {
      LoadBlockData(BlockType::Air,         "../data/block/00_air.json");
      LoadBlockData(BlockType::Grass,       "../data/block/01_grass.json");
      LoadBlockData(BlockType::Dirt,        "../data/block/02_dirt.json");
      LoadBlockData(BlockType::Stone,       "../data/block/03_stone.json");
      LoadBlockData(BlockType::Sand,        "../data/block/04_sand.json");
      LoadBlockData(BlockType::Log,         "../data/block/05_log.json");
      LoadBlockData(BlockType::Snow,        "../data/block/06_snow.json");
      LoadBlockData(BlockType::Water,       "../data/block/07_water.json");
      LoadBlockData(BlockType::Sandstone,   "../data/block/08_sandstone.json");
      LoadBlockData(BlockType::Glass,       "../data/block/09_glass.json");
      LoadBlockData(BlockType::Leaves,      "../data/block/10_leaves.json");
      LoadBlockData(BlockType::TallGrass,   "../data/block/11_tallgrass.json");
    }

    void BlockData::LoadBlockData(BlockType block, const std::string &path) {
      std::ifstream file(path);

      if (!file.is_open()) {
        Utils::Logger::Error("Json: file cannot open");
        exit(1);
      }

      nlohmann::json json;
      file >> json;

      if (
        !json["name"].is_string() ||
        !json["id"].is_number() ||
        !json["renderType"].is_string() ||
        !json["isSolid"].is_boolean() ||
        !json["isTranslucent"].is_boolean()
      ) {
        Utils::Logger::Error("Invalid block data at path {}: Missing attributes.", path);
        exit(1);
      }

      std::string name = json["name"];
      int id = json["id"];
      if (id < 0) {
        Utils::Logger::Error("Invalid block data at path {}: Invalid id.", path);
        exit(1);
      }

      BlockRenderType renderType;
      if (json["renderType"] == "empty") {
        renderType = BlockRenderType::Empty;
      } else if (json["renderType"] == "standard") {
        renderType = BlockRenderType::Standard;
      } else if (json["renderType"] == "fluid") {
        renderType = BlockRenderType::Fluid;
      } else if (json["renderType"] == "foliage") {
        renderType = BlockRenderType::Foliage;
      } else {
        Utils::Logger::Error("Invalid block data at path {}: Invalid render type \"{}\"", path, json["renderType"]);
        exit(1);
      }

      bool isSolid = json["isSolid"];
      bool isTranslucent = json["isTranslucent"];

      data[block] = BlockDefinition{block, name, id, renderType, isSolid, isTranslucent};
    }

  }

  auto operator<<(std::ostream &os, BlockType type) -> std::ostream & {
    os << World::BlockData::Get(type).name;
    return os;
  }

}
