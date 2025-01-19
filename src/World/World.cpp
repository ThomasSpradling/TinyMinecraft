#include "World/World.h"
#include "World/Block.h"
#include "World/Chunk.h"
#include "glm/fwd.hpp"
#include <algorithm>
#include <iostream>

namespace World {

void World::Generate() {

  // for (glm::ivec2 offset : { glm::ivec2(0, 0), glm::ivec2(-1, 0), glm::ivec2(0, -1), glm::ivec2(-1, -1) }) {

  //   GenerateChunk(offset);

  //   // Chunk chunk { *this, offset };
  //   // chunk.Init();
  //   // for (GLuint x = 0; x < CHUNK_WIDTH; ++x) {
  //   //   for (GLuint y = 0; y < CHUNK_HEIGHT; ++y) {
  //   //     for (GLuint z = 0; z < CHUNK_LENGTH; ++z) {
  //   //       if (y <= 27) {
  //   //         chunk.SetBlockAt(x, y, z, Block(BlockType::STONE));
  //   //       }
  //   //       if (y > 27 && y <= 31) {
  //   //         chunk.SetBlockAt(x, y, z, Block(BlockType::DIRT));
  //   //       }
  //   //       if (y == 32) {
  //   //         chunk.SetBlockAt(x, y, z, Block(BlockType::GRASS));
  //   //       }
  //   //     }
  //   //   }
  //   // }

  //   // chunks.emplace(offset, chunk);
  // }
}

Chunk &World::GenerateChunk(glm::ivec2 &offset) {
  Chunk chunk {*this, offset};

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

  return chunks.at(offset);
}

std::vector<glm::ivec2> World::GetNearbyChunks(glm::vec3 &pos, int radius) {
  glm::ivec2 chunkPos = GetChunkPosFromCoords(pos);

  std::vector<glm::ivec2> chunks;

  for (int dx = -radius; dx <= radius; ++dx) {
    for (int dz = -radius; dz <= radius; ++dz) {
      if (dx*dx + dz*dz <= radius * radius) {
        chunks.push_back({ chunkPos.x + dx, chunkPos.y + dz });
      }
    }
  }

  return chunks;
}

void World::Update(glm::vec3 &playerPos) {
  for (glm::ivec2 &chunkPos : GetNearbyChunks(playerPos, generateRadius)) {
    if (!chunks.contains(chunkPos)) {
      GenerateChunk(chunkPos);
    }
  }

  // Make all currently visible chunks go hidden
  for (const glm::ivec2 &chunkPos : visibleChunks) {
    Chunk &chunk = chunks.at(chunkPos);
    chunk.SetHidden(true);
  }
  visibleChunks.clear();

  // Make visible all nearby chunks--and also load chunks that have not been loaded before
  for (glm::ivec2 &chunkPos : GetNearbyChunks(playerPos, renderRadius)) {
    Chunk &chunk = chunks.at(chunkPos);

    // std::cout << "CHUNK UPDATE BUFFER: " << chunkUpdateBuffer.size() << std::endl;
    if (chunk.GetState() == ChunkState::Unloaded) {
      chunkUpdateBuffer.push_back(chunkPos);
      chunk.SetState(ChunkState::Loading);
    } else {
      chunk.SetHidden(false);
      visibleChunks.insert(chunkPos);
    }
  }

  if (!chunkUpdateBuffer.empty()) {
    Chunk &chunk = chunks.at(chunkUpdateBuffer.front());
    chunkUpdateBuffer.pop_front();

    chunk.UpdateMesh();

    chunk.SetState(ChunkState::Loaded);
  }
}

// void World::UpdateChunk(Chunk &chunk) {
//   if (chunk.IsLoaded()) {
//     chunkUpdateBuffer.push_back(chunk.GetChunkPos());
//   }
// }

std::unordered_map<glm::ivec2, Chunk, Utils::IVec2Hash> &World::GetChunks() {
  return chunks;
}


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

  // if not in a chunk
  if (neighborPos.y >= CHUNK_HEIGHT || neighborPos.y < 0 ||
  !chunks.contains(neighborChunkPos)) {
    return true;
  }

  glm::vec3 neightborLocalPos = GetLocalBlockCoords(neighborPos);

  if (chunks.at(neighborChunkPos).GetBlockAt(neightborLocalPos).IsSolid()) {
    return false;
  }

  return true;
}

// bool World::IsChunkNearby(glm::vec3 &playerPos, glm::ivec2 chunkPos) {
//   glm::ivec2 playerChunkPos = GetChunkPosFromCoords(playerPos);
  
//   int dx = glm::abs(playerChunkPos.x - chunkPos.x);
//   int dz = glm::abs(playerChunkPos.y - chunkPos.y);

//   return dx * dx + dz * dz <= radius * radius;
// }

}
