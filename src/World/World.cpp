#include "World/World.h"
#include "World/Block.h"

namespace World {

void World::Generate() {

  for (const glm::vec2 &offset : { glm::vec2(0, 0), glm::vec2(-1, 0), glm::vec2(0, -1), glm::vec2(-1, -1) }) {

    Chunk chunk { offset };
    chunk.Init();
    for (GLuint x = 0; x < CHUNK_WIDTH; ++x) {
      for (GLuint y = 0; y < CHUNK_HEIGHT; ++y) {
        for (GLuint z = 0; z < CHUNK_LENGTH; ++z) {
          if (y <= 32) {
            chunk.SetBlockAt(x, y, z, Block(BlockType::DIRT));
          }
        }
      }
    }
    chunks.push_back(chunk);
  }

  // chunks.push_back(chunk);
  // chunks.push_back(chunk);
  // chunks.push_back(chunk);
  // chunks.push_back(chunk);
}

void World::Update() {
  for (Chunk &chunk : chunks) {
    if (chunk.IsDirty()) {
      chunk.UpdateMesh();
      chunk.SetDirty(false);
    }
  }
}

std::vector<Chunk> &World::GetChunks() {
  return chunks;
}

}
