#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <memory>
#include <optional>
#include "World/World.h"
#include "Utils/Profiler.h"
#include "Utils/defs.h"
#include "Utils/noise.h"
#include "Utils/utils.h"
#include "World/Biome.h"
#include "World/Block.h"
#include "World/Chunk.h"
#include "glm/ext/vector_int2.hpp"

namespace World {

World::World() : temperatureMap(seed + 1), humidityMap(seed + 2), blendMap(seed + 3), heightMap(seed + 4), stoneMap(seed + 5), worldGen(*this) {}

void World::Initialize() {}

void World::GenerateChunk(const glm::ivec2 &chunkPos) {
  PROFILE_FUNCTION(Chunk)

  // worldGen.GenerateTerrainChunk(chunkPos, blendMap, heightMap, stoneMap);
}

// Chunk &World::CreateEmptyChunk(const glm::ivec2 &chunkPos) {
//   Chunk chunk { *this, chunkPos };
//   chunk.Initialize();

//   chunks.emplace(chunkPos, chunk);
//   return chunks.at(chunkPos);
// }

std::vector<glm::ivec2> World::GetNearbyChunks(const glm::vec3 &pos, int radius) {
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

float World::GetTemperature(int x, int z) {
  const float temperatureScale = 0.001;

  float temperature = Utils::OctaveNoise(x * temperatureScale, z * temperatureScale, temperatureMap);
  temperature = Utils::ScaleValue(0.3, 0.7, 0.1, 0.9, temperature);
  temperature = std::clamp(temperature, 0.f, 1.f);

  return temperature;
}

float World::GetHumidity(int x, int z) {
  const float humidityScale = 0.003;

  float humidity = Utils::OctaveNoise(x * humidityScale, z * humidityScale, humidityMap);
  humidity = Utils::ScaleValue(0.3, 0.7, 0.1, 0.9, humidity);
  humidity = std::clamp(humidity, 0.f, 1.f);

  return  humidity;
}

BiomeType World::GetBiome(int x, int z) {
  return worldGen.SelectBiomes(GetTemperature(x, z), GetHumidity(x, z)).first->GetType();
}

void World::Update(const glm::vec3 &playerPos) {
  PROFILE_FUNCTION(Chunk)

  const glm::ivec2 playerChunkPos = GetChunkPosFromCoords(playerPos);

  constexpr int viewRadius = GFX_RENDER_DISTANCE;
  constexpr int loadRadius = viewRadius + 1;

  std::vector<glm::ivec2> chunksToUnload;

  for (auto &[chunkPos, chunk] : loadedChunks) {
    // if not nearby

    int dx = chunkPos.x - playerChunkPos.x;
    int dz = chunkPos.y - playerChunkPos.y;

    float distanceSquared = dx * dx + dz * dz;

    if (distanceSquared > loadRadius * loadRadius) {
      chunksToUnload.push_back(chunkPos);
    }
  }

  for (const auto &chunkPos : chunksToUnload) {
    UnloadChunk(loadedChunks.at(chunkPos));
  }

  std::cout << "SIZE 2: " << loadedChunks.size() << std::endl;

  for (int dz = -loadRadius; dz <= loadRadius; ++dz) {
    for (int dx = -loadRadius; dx <= loadRadius; ++dx) {
      float distanceSquared = dx * dx + dz * dz;

      if (distanceSquared <= loadRadius * loadRadius) {
        glm::ivec2 chunkPos { playerChunkPos.x + dx, playerChunkPos.y + dz };

        if (!loadedChunks.contains(chunkPos)) {
          LoadChunk(chunkPos.x, chunkPos.y);
        }
      }
    }
  }

  // }

  std::cout << "SIZE 3: " << loadedChunks.size() << std::endl;

  // Figure out closet dirty chunk in range

  float shortestDistance = std::numeric_limits<float>::max();
  Chunk *closestDirtyChunk = nullptr;

  for (auto &[chunkPos, chunk] : loadedChunks) {
    int dx = chunkPos.x - playerChunkPos.x;
    int dz = chunkPos.y - playerChunkPos.y;

    float distanceSquared = dx * dx + dz * dz;

    if (distanceSquared <= viewRadius * viewRadius && chunk.IsDirty()) {
      if (distanceSquared < shortestDistance) {
        closestDirtyChunk = &chunk;
        shortestDistance = distanceSquared;
      }
    }
  }


  // if there is a closet dirty chunk in range, update its mesh

  if (closestDirtyChunk) {
    closestDirtyChunk->UpdateMesh();
    closestDirtyChunk->SetDirty(false);
  }

  // Loading chunk refers to "generating it" or "loading from disk / db"
  // Updating chunk refers to "remeshing it" and "assigning blocks"

  // for each currentChunks:
  //    unloadChunk()
  // 
  //  currentChunks.clear() 
  //
  // for each GetNearbyChunks(radius)
  //    loadChunk()
  //    currentChunk.push()
  //
  // find nearest chunk and update that

  // const auto nearbyChunks = GetNearbyChunks(playerPos, renderRadius);
  // const auto nearbyRenderChunks = GetNearbyChunks(playerPos, renderRadius);

  // for (const glm::ivec2 &chunkPos : nearbyChunks) {
  //   if (!chunks.contains(chunkPos)) {
  //     GenerateChunk(chunkPos);
  //   }
  // }

  // // Make all currently visible chunks go hidden
  // for (const glm::ivec2 &chunkPos : visibleChunks) {
  //   Chunk &chunk = chunks.at(chunkPos);
  //   chunk.SetHidden(true);
  // }
  // visibleChunks.clear();

  // // Make visible all nearby chunks--and also load chunks that have not been loaded before
  // for (const glm::ivec2 &chunkPos : nearbyRenderChunks) {
  //   Chunk &chunk = chunks.at(chunkPos);

  //   if (chunk.GetState() == ChunkState::Unloaded) {
  //     chunkUpdateBuffer.push_back(chunkPos);
  //     chunk.SetState(ChunkState::Loading);
  //   } else {
  //     chunk.SetHidden(false);
  //     visibleChunks.insert(chunkPos);
  //   }
  // }

  // if (chunkUpdateBuffer.empty()) {
  //   return;
  // }

  // float minDistance = std::numeric_limits<double>::infinity();
  // glm::ivec2 nearestChunkPos;
  // for (const auto &chunkPos : chunkUpdateBuffer) {
  //   float distance = glm::distance(glm::vec2(GetChunkPosFromCoords(playerPos)), glm::vec2(chunkPos));
  //   if (distance < minDistance) {
  //     minDistance = distance;
  //     nearestChunkPos = chunkPos;
  //   }
  // }

  // if (minDistance != std::numeric_limits<float>::infinity()) {
  //   Chunk &chunk = chunks.at(nearestChunkPos);
  //   chunk.UpdateMesh();
  //   chunk.SetState(ChunkState::Loaded);

  //   chunkUpdateBuffer.erase(std::remove(chunkUpdateBuffer.begin(), chunkUpdateBuffer.end(), nearestChunkPos), chunkUpdateBuffer.end());
  // }
}

void World::UnloadChunk(int x, int z) {
  const glm::ivec2 chunkPos { x, z };

  if (loadedChunks.contains(chunkPos)) {

    // Store into cache / persistent storage

    loadedChunks.erase(chunkPos);
  }
}

void World::UnloadChunk(Chunk &chunk) {
  const glm::ivec2 &chunkPos = chunk.GetChunkPos();

  UnloadChunk(chunkPos.x, chunkPos.y);
}

void World::LoadChunk(int x, int z) {
  const glm::ivec2 chunkPos { x, z };

  // Check cache / persistent storage. If not there, then move on

  Chunk chunk { *this, chunkPos };
  chunk.Initialize();
  chunk.SetState(ChunkState::Loaded);

  worldGen.GenerateTerrainChunk(chunk, blendMap, heightMap, stoneMap);
  loadedChunks.emplace(chunkPos, chunk);
}

void World::LoadChunk(Chunk &chunk) {
  if (chunk.GetState() == ChunkState::Loaded) {
    return;
  }
  
  chunk.SetDirty(true);

  const glm::ivec2 &chunkPos = chunk.GetChunkPos();
  if (!loadedChunks.contains(chunkPos)) {
    worldGen.GenerateTerrainChunk(chunk, blendMap, heightMap, stoneMap);
    loadedChunks.emplace(chunkPos, chunk);
  }
  chunk.SetState(ChunkState::Loaded);
}

Chunk &World::GetChunkAt(int x, int z) {
  return GetChunkAt(glm::ivec2(x, z));
}

Chunk &World::GetChunkAt(const glm::ivec2 &chunkPos) {
  if (loadedChunks.contains(chunkPos)) {
    return loadedChunks.at(chunkPos);
  }
  // More advanced logic here involving loading it first
}

std::unordered_map<glm::ivec2, Chunk, Utils::IVec2Hash> &World::GetLoadedChunks() {
  return loadedChunks;
}

// void World::BreakBlock(const glm::vec3 &pos) {
//   SetBlockAt(pos, BlockType::AIR);

//   chunks.at(GetChunkPosFromCoords(pos)).UpdateMesh();
// }

void World::SetBlockAt(const glm::vec3 &pos, BlockType type) {
  Block &block = GetBlockAt(pos);
  block.SetType(type);
}

Block &World::GetBlockAt(const glm::vec3 &pos) {
  glm::ivec2 chunkPos = GetChunkPosFromCoords(pos);
  glm::vec3 offsetPos = GetLocalBlockCoords(pos);

  return loadedChunks.at(chunkPos).GetBlockAt(offsetPos);
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
  !loadedChunks.contains(neighborChunkPos)) {
    return true;
  }

  glm::vec3 neightborLocalPos = GetLocalBlockCoords(neighborPos);

  if (loadedChunks.at(neighborChunkPos).GetBlockAt(neightborLocalPos).IsSolid()) {
    return false;
  }

  return true;
}



}
