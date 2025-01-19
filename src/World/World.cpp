#include "World/World.h"
#include "World/Block.h"
#include "World/Chunk.h"
#include <iostream>
#include <memory>

namespace World {

void World::Generate() {

  for (glm::ivec2 offset : { glm::ivec2(0, 0), glm::ivec2(-1, 0), glm::ivec2(0, -1), glm::ivec2(-1, -1) }) {

    Chunk chunk { *this, offset };
    chunk.Init();
    for (GLuint x = 0; x < CHUNK_WIDTH; ++x) {
      for (GLuint y = 0; y < CHUNK_HEIGHT; ++y) {
        for (GLuint z = 0; z < CHUNK_LENGTH; ++z) {
          if (y <= 27) {
            chunk.SetBlockAt(x, y, z, Block(BlockType::STONE));
          }
          if (y > 27 && y <= 31) {
            chunk.SetBlockAt(x, y, z, Block(BlockType::DIRT));
          }
          if (y == 32) {
            chunk.SetBlockAt(x, y, z, Block(BlockType::GRASS));
          }
        }
      }
    }

    chunks.emplace(offset, chunk);
  }
}

void World::Update() {
  for (auto &[offset, chunk] : chunks) {
    if (chunk.IsDirty()) {
      chunk.UpdateMesh();
      chunk.SetDirty(false);
    }
  }
}

std::unordered_map<glm::ivec2, Chunk, Utils::IVec2Hash> &World::GetChunks() {
  return chunks;
}

// Chunk &World::GetChunkAt(int x, int z) {
//   return chunks.at({ x, z });
// }
// Chunk &World::GetChunkAt(const glm::ivec2 &pos) {
//   return chunks.at({ pos.x, pos.y });
// }

// Block &World::GetBlockAt(int x, int y, int z) {
//   Chunk &chunk = GetChunkAt(GetChunkPosFromCoords({ x,y,z }));

//   int offsetX;
//   if (x % CHUNK_WIDTH == 0) {
//     offsetX = 0;
//   } else {
//     offsetX = x % CHUNK_WIDTH + (x > 0 ? 0 : CHUNK_WIDTH);
//   }

//   int offsetZ;
//   if (x % CHUNK_WIDTH == 0) {
//     offsetZ = 0;
//   } else {
//     offsetZ = z % CHUNK_LENGTH + (z > 0 ? 0 : CHUNK_LENGTH);
//   }

//   return chunk.GetBlockAt(offsetX, y, offsetZ);
// }

// Block &World::GetBlockAt(const glm::vec3 &pos) {
//   return GetBlockAt(pos.x, pos.y, pos.z);
// }

glm::vec3 World::GetLocalBlockCoords(const glm::vec3 &pos) {
  int localX = static_cast<int>(pos.x) % CHUNK_WIDTH;
  int localY = static_cast<int>(pos.y);
  int localZ = static_cast<int>(pos.z) % CHUNK_LENGTH;

  // Handle negative coordinates to ensure proper wrapping
  if (localX < 0) localX += CHUNK_WIDTH;
  if (localZ < 0) localZ += CHUNK_LENGTH;

  return glm::vec3(localX, localY, localZ);
}

glm::ivec2 World::GetChunkPosFromCoords(const glm::vec3 &pos) {
  int offsetX = static_cast<int>(std::floor(pos.x / CHUNK_WIDTH));
  int offsetZ = static_cast<int>(std::floor(pos.z / CHUNK_LENGTH));

  return { offsetX, offsetZ };
}

bool World::IsFaceVisible(BlockFace face, const glm::vec3 &pos) {
  glm::vec3 neighborPos = pos + face.GetNormal();

  glm::ivec2 neighborChunkPos = GetChunkPosFromCoords(neighborPos);

  // if not in a loaded chunk
  if (neighborPos.y >= CHUNK_HEIGHT || neighborPos.y < 0 ||
  !chunks.contains(neighborChunkPos) || !chunks.at(neighborChunkPos).IsLoaded()) {
    return true;
  }

  glm::vec3 neightborLocalPos = GetLocalBlockCoords(neighborPos);

  if (chunks.at(neighborChunkPos).GetBlockAt(neightborLocalPos).IsSolid()) {
    return false;
  }

  return true;
}

}
