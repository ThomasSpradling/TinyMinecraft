#include <PerlinNoise/PerlinNoise.h>
#include <algorithm>
#include <cmath>
#include "World/World.h"
#include "Utils/Profiler.h"
#include "Utils/defs.h"
#include "World/Block.h"
#include "World/Chunk.h"
#include "glm/ext/vector_int2.hpp"

namespace World {

void World::Initialize() {}

Chunk &World::GenerateChunk(glm::ivec2 &chunkPos) {
  PROFILE_FUNCTION(Chunk)

  Chunk chunk { *this, chunkPos };
  chunk.Initialize();

  const double scale = 0.01;
  const double slice = 42.0;

  PerlinNoise pn { seed };

  const int averageHeight = 80;
  const int dirtLayerDepth = 5;

  for (int x = 0; x < CHUNK_WIDTH; ++x) {
    for (int z = 0; z < CHUNK_LENGTH; ++z) {
      int globalX = chunkPos.x * static_cast<int>(CHUNK_WIDTH) + x;
      int globalZ = chunkPos.y * static_cast<int>(CHUNK_LENGTH) + z;

      double noiseValue = pn.noise(globalX * scale, slice, globalZ * scale);
      noiseValue = (noiseValue + 1.0) / 2.0;
      int height = static_cast<int>(std::floor(noiseValue * averageHeight));
      height = std::clamp(height, 0, CHUNK_HEIGHT - 1);

      for (int y = 0; y <= height; ++y) {
        if (y == height) {
          chunk.SetBlockAt(glm::vec3(x, y, z), BlockType::GRASS);
        } else if (y >= height - dirtLayerDepth) {
          chunk.SetBlockAt(glm::vec3(x, y, z), BlockType::DIRT);
        } else {
          chunk.SetBlockAt(glm::vec3(x, y, z), BlockType::STONE);
        }
      }
    }
  }

  chunks.emplace(chunkPos, chunk);
  return chunks.at(chunkPos);
}

std::vector<glm::ivec2> World::GetNearbyChunks(glm::vec3 &pos, int radius) {
  glm::ivec2 chunkPos = GetChunkPosFromCoords(pos);

  std::vector<glm::ivec2> res;

  for (int dx = -radius; dx <= radius; ++dx) {
    for (int dz = -radius; dz <= radius; ++dz) {
      if (dx*dx + dz*dz <= radius * radius) {
        res.push_back({ chunkPos.x + dx, chunkPos.y + dz });
      }
    }
  }

  return res;
}

void World::Update(glm::vec3 &playerPos) {
  PROFILE_FUNCTION(Chunk)

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

    if (chunk.GetState() == ChunkState::Unloaded) {
      chunkUpdateBuffer.push_back(chunkPos);
      chunk.SetState(ChunkState::Loading);
    } else {
      chunk.SetHidden(false);
      visibleChunks.insert(chunkPos);
    }
  }

  if (chunkUpdateBuffer.empty()) {
    return;
  }

  float minDistance = std::numeric_limits<double>::infinity();
  glm::ivec2 nearestChunkPos;
  for (const auto &chunkPos : chunkUpdateBuffer) {
    float distance = glm::distance(glm::vec2(GetChunkPosFromCoords(playerPos)), glm::vec2(chunkPos));
    if (distance < minDistance) {
      minDistance = distance;
      nearestChunkPos = chunkPos;
    }
  }

  if (minDistance != std::numeric_limits<float>::infinity()) {
    Chunk &chunk = chunks.at(nearestChunkPos);
    chunk.UpdateMesh();
    chunk.SetState(ChunkState::Loaded);

    chunkUpdateBuffer.erase(std::remove(chunkUpdateBuffer.begin(), chunkUpdateBuffer.end(), nearestChunkPos), chunkUpdateBuffer.end());
  }
}

std::unordered_map<glm::ivec2, Chunk, Utils::IVec2Hash> &World::GetChunks() {
  return chunks;
}

void World::BreakBlock(const glm::vec3 &pos) {
  SetBlockAt(pos, BlockType::AIR);

  chunks.at(GetChunkPosFromCoords(pos)).UpdateMesh();
}

void World::SetBlockAt(const glm::vec3 &pos, BlockType type) {
  Block &block = GetBlockAt(pos);
  block.SetType(type);
}

Block &World::GetBlockAt(const glm::vec3 &pos) {
  glm::ivec2 chunkPos = GetChunkPosFromCoords(pos);
  glm::vec3 offsetPos = GetLocalBlockCoords(pos);

  return chunks.at(chunkPos).GetBlockAt(offsetPos);
}

bool World::HasBlock(const glm::vec3 &pos) {
  return GetBlockAt(pos).IsSolid();
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



}
