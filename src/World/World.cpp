#include <algorithm>
#include <array>
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

namespace World {

World::World() : temperatureMap(seed + 1), humidityMap(seed + 2), blendMap(seed + 3), heightMap(seed + 4), stoneMap(seed + 5), treeMap(seed + 6), grassMap(seed + 7), worldGen(*this) {}

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

void World::Render(Graphics::Texture &blockAtlasTexture, GLuint depthMap, Graphics::Shader &blockShader, Graphics::Shader &waterShader, const glm::vec3 &playerPos) {
  PROFILE_FUNCTION(World)
  
  std::vector<Chunk *> translucentChunks;
  
  for (auto &[offset, chunk] : GetLoadedChunks()) {
    if (chunk.HasTranslucentBlocks()) {
      translucentChunks.push_back(&chunk);
    }

    chunk.Render(blockAtlasTexture, depthMap, blockShader, playerPos);
  }

  glm::vec2 playerChunkPos = GetChunkPosFromCoords(playerPos);

  std::sort(translucentChunks.begin(), translucentChunks.end(), [playerChunkPos](Chunk *a, Chunk *b) {
    float distanceA = glm::distance(playerChunkPos, static_cast<glm::vec2>(a->GetChunkPos()));
    float distanceB = glm::distance(playerChunkPos, static_cast<glm::vec2>(b->GetChunkPos()));

    return distanceA > distanceB;
  });

  for (auto chunk : translucentChunks) {
    chunk->RenderTranslucent(blockAtlasTexture, depthMap, blockShader, playerPos);
  }
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

  for (int dz = -loadRadius; dz <= loadRadius; ++dz) {
    for (int dx = -loadRadius; dx <= loadRadius; ++dx) {
      float distanceSquared = dx * dx + dz * dz;

      if (distanceSquared <= loadRadius * loadRadius) {
        glm::ivec2 chunkPos { playerChunkPos.x + dx, playerChunkPos.y + dz };

        worldGen.LoadUnloadedBlocks(GetChunkAt(chunkPos));
      }
    }
  }

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
    closestDirtyChunk->UpdateTranslucentMesh(playerPos);
    closestDirtyChunk->SetDirty(false);
  }
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
  worldGen.GenerateFeatures(chunk, treeMap, grassMap);

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
    worldGen.GenerateFeatures(chunk, treeMap, grassMap);

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
  // TODO: More advanced logic here involving loading it first
}

std::unordered_map<glm::ivec2, Chunk, Utils::IVec2Hash> &World::GetLoadedChunks() {
  return loadedChunks;
}

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
  return GetBlockAt(pos).GetType() != BlockType::AIR;
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

void World::HandlePlayerMovement(const glm::vec3 &before, const glm::vec3 &after) {
  if (before == after) {
    return;
  }

  glm::ivec2 playerChunkPosBefore = GetChunkPosFromCoords(before);
  glm::ivec2 playerChunkPosAfter = GetChunkPosFromCoords(after);

  std::array<glm::ivec2, 9> directions = {
    glm::ivec2(1, 0), glm::ivec2(-1, 0), glm::ivec2(0, 1),
    glm::ivec2(0, -1), glm::ivec2(1, -1), glm::ivec2(1, 1),
    glm::ivec2(-1, 1), glm::ivec2(-1, -1), glm::ivec2(0, 0)
  };

  // if moving within same chunk, always re-sort that chunk
  if (playerChunkPosBefore == playerChunkPosAfter) {
    if (IsChunkLoaded(playerChunkPosBefore)) {
      GetChunkAt(playerChunkPosBefore).SortTranslucentBlocks(after);
    }
  } else {
    // GetChunkAt(playerChunkPosBefore).SortTranslucentBlocks(after);
    for (const auto &offset : directions) {
      if (IsChunkLoaded(playerChunkPosBefore + offset)) {
        GetChunkAt(playerChunkPosBefore + offset).SortTranslucentBlocks(after);
      }
    }
  }


}

bool World::IsChunkLoaded(const glm::ivec2 &pos) {
  return loadedChunks.contains(pos);
}

bool World::IsFaceVisible(const Block &block, BlockFace face, const glm::vec3 &pos) {
  glm::vec3 neighborPos = pos + face.GetNormal();
  glm::ivec2 neighborChunkPos = GetChunkPosFromCoords(neighborPos);

  // if not in a chunk
  if (neighborPos.y >= CHUNK_HEIGHT || neighborPos.y < 0 ||
  !loadedChunks.contains(neighborChunkPos)) {
    return true;
  }

  glm::vec3 neightborLocalPos = GetLocalBlockCoords(neighborPos);

  Block neightboringBlock = loadedChunks.at(neighborChunkPos).GetBlockAt(neightborLocalPos);

  if (neightboringBlock.GetType() == BlockType::AIR) {
    return true;
  }

  if (!block.IsTransparent()) {
    return neightboringBlock.IsTransparent();
  }

  if (block.IsTransparent()) {
    if (neightboringBlock.IsTransparent()) {
      return neightboringBlock.GetType() != block.GetType();
    }
  }

  return false;
}


}
