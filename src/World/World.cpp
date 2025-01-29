#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include "World/World.h"
#include "Geometry/geometry.h"
#include "Utils/Profiler.h"
#include "Utils/defs.h"
#include "Utils/noise.h"
#include "Utils/utils.h"
#include "World/Biome.h"
#include "World/Block.h"
#include "World/Chunk.h"

namespace World {

World::World()
  : m_temperatureMap(seed + 1)
  , m_humidityMap(seed + 2)
  , m_blendMap(seed + 3)
  , m_heightMap(seed + 4)
  , m_stoneMap(seed + 5)
  , m_treeMap(seed + 6)
  , m_grassMap(seed + 7)
  , m_worldGen(*this)
{}

auto World::GetTemperature(int x, int z) -> double {
  const double temperatureScale = 0.001;

  double temperature = Utils::OctaveNoise(x * temperatureScale, z * temperatureScale, m_temperatureMap);
  temperature = Utils::ScaleValue(0.3, 0.7, 0.1, 0.9, temperature);
  temperature = std::clamp(temperature, 0.0, 1.0);

  return temperature;
}

auto World::GetHumidity(int x, int z) -> double {
  const double humidityScale = 0.003;

  double humidity = Utils::OctaveNoise(x * humidityScale, z * humidityScale, m_humidityMap);
  humidity = Utils::ScaleValue(0.3, 0.7, 0.1, 0.9, humidity);
  humidity = std::clamp(humidity, 0.0, 1.0);

  return humidity;
}

auto World::GetBiome(int x, int z) -> BiomeType {
  return m_worldGen.SelectBiomes(GetTemperature(x, z), GetHumidity(x, z)).first->GetType();
}

void World::Update(const glm::vec3 &playerPos) {
  PROFILE_FUNCTION(Chunk)

  const glm::ivec2 playerChunkPos = GetChunkPosFromCoords(playerPos);

  constexpr int viewRadius = GFX_RENDER_DISTANCE;
  constexpr int loadRadius = viewRadius + 1;

  std::vector<glm::ivec2> chunksToUnload;

  for (auto &[chunkPos, chunk] : m_loadedChunks) {
    // if not nearby

    int dx = chunkPos.x - playerChunkPos.x;
    int dz = chunkPos.y - playerChunkPos.y;

    int distanceSquared = dx * dx + dz * dz;

    if (distanceSquared > loadRadius * loadRadius) {
      chunksToUnload.push_back(chunkPos);
    }
  }

  for (const auto &chunkPos : chunksToUnload) {
    UnloadChunk(m_loadedChunks.at(chunkPos));
  }

  for (int dz = -loadRadius; dz <= loadRadius; ++dz) {
    for (int dx = -loadRadius; dx <= loadRadius; ++dx) {
      int distanceSquared = dx * dx + dz * dz;

      if (distanceSquared <= loadRadius * loadRadius) {
        glm::ivec2 chunkPos { playerChunkPos.x + dx, playerChunkPos.y + dz };

        if (!m_loadedChunks.contains(chunkPos)) {
          LoadChunk(chunkPos.x, chunkPos.y);
        }
      }
    }
  }

  for (int dz = -loadRadius; dz <= loadRadius; ++dz) {
    for (int dx = -loadRadius; dx <= loadRadius; ++dx) {
      int distanceSquared = dx * dx + dz * dz;

      if (distanceSquared <= loadRadius * loadRadius) {
        glm::ivec2 chunkPos { playerChunkPos.x + dx, playerChunkPos.y + dz };

        m_worldGen.LoadUnloadedBlocks(GetChunkAt(chunkPos));
      }
    }
  }

  // Figure out closet dirty chunk in range

  int shortestDistance = std::numeric_limits<int>::max();
  Chunk *closestDirtyChunk = nullptr;

  for (auto &[chunkPos, chunk] : m_loadedChunks) {
    int dx = chunkPos.x - playerChunkPos.x;
    int dz = chunkPos.y - playerChunkPos.y;

    int distanceSquared = dx * dx + dz * dz;

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

  if (m_loadedChunks.contains(chunkPos)) {

    // Store into cache / persistent storage

    m_loadedChunks.erase(chunkPos);
  }
}

void World::UnloadChunk(Chunk &chunk) {
  const glm::ivec2 &chunkPos = chunk.GetChunkPos();

  UnloadChunk(chunkPos.x, chunkPos.y);
}

void World::LoadChunk(int x, int z) {
  const glm::ivec2 chunkPos { x, z };

  // Check cache / persistent storage. If not there, then move on

  m_loadedChunks.emplace(chunkPos, Chunk(*this, chunkPos));

  m_loadedChunks.at(chunkPos).SetState(ChunkState::Loaded);
  m_worldGen.GenerateTerrainChunk(m_loadedChunks.at(chunkPos), m_blendMap, m_heightMap, m_stoneMap);
  m_worldGen.GenerateFeatures(m_loadedChunks.at(chunkPos), m_treeMap, m_grassMap);

  // Utils::g_logger.Debug("Hello??");
}

void World::LoadChunk(Chunk &chunk) {
  if (chunk.GetState() == ChunkState::Loaded) {
    return;
  }
  
  chunk.SetDirty(true);

  const glm::ivec2 &chunkPos = chunk.GetChunkPos();
  if (!m_loadedChunks.contains(chunkPos)) {
    m_worldGen.GenerateTerrainChunk(chunk, m_blendMap, m_heightMap, m_stoneMap);
    m_worldGen.GenerateFeatures(chunk, m_treeMap, m_grassMap);

    m_loadedChunks.emplace(chunkPos, std::move(chunk));
  }
  chunk.SetState(ChunkState::Loaded);
}

auto World::GetChunkAt(int x, int z) -> Chunk & {
  return GetChunkAt(glm::ivec2(x, z));
}

auto World::GetChunkAt(const glm::ivec2 &chunkPos) -> Chunk & {
  if (m_loadedChunks.contains(chunkPos)) {
    return m_loadedChunks.at(chunkPos);
  }
  // TODO: More advanced logic here involving loading it first
}

auto World::GetLoadedChunks() -> std::unordered_map<glm::ivec2, Chunk, Utils::IVec2Hash> & {
  return m_loadedChunks;
}

void World::SetBlockAt(const glm::vec3 &pos, BlockType type) {
  Block &block = GetBlockAt(pos);
  block.SetType(type);
}

auto World::GetBlockAt(const glm::vec3 &pos) -> Block & {
  glm::ivec2 chunkPos = GetChunkPosFromCoords(pos);
  glm::vec3 offsetPos = GetLocalBlockCoords(pos);

  return m_loadedChunks.at(chunkPos).GetBlockAt(offsetPos);
}

auto World::HasBlock(const glm::vec3 &pos) -> bool {
  return GetBlockAt(pos).GetType() != BlockType::AIR;
}

auto World::GetLocalBlockCoords(const glm::vec3 &pos) -> glm::vec3 {
  int localX = static_cast<int>(pos.x) % CHUNK_WIDTH;
  int localY = static_cast<int>(pos.y);
  int localZ = static_cast<int>(pos.z) % CHUNK_LENGTH;

  // Handle negative coordinates to ensure proper wrapping
  if (localX < 0) localX += CHUNK_WIDTH;
  if (localZ < 0) localZ += CHUNK_LENGTH;

  return {localX, localY, localZ};
}

auto World::GetChunkPosFromCoords(const glm::vec3 &pos) -> glm::ivec2 {
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
    for (const auto &offset : directions) {
      if (IsChunkLoaded(playerChunkPosBefore + offset)) {
        GetChunkAt(playerChunkPosBefore + offset).SortTranslucentBlocks(after);
      }
    }
  }
}

auto World::IsChunkLoaded(const glm::ivec2 &pos) -> bool {
  return m_loadedChunks.contains(pos);
}

auto World::IsFaceVisible(const Block &block, Geometry::Face face, const glm::vec3 &pos) -> bool {
  glm::vec3 neighborPos = pos + Geometry::GetNormal(face);
  glm::ivec2 neighborChunkPos = GetChunkPosFromCoords(neighborPos);

  // if not in a chunk
  if (neighborPos.y >= CHUNK_HEIGHT || neighborPos.y < 0 ||
  !m_loadedChunks.contains(neighborChunkPos)) {
    return true;
  }

  glm::vec3 neightborLocalPos = GetLocalBlockCoords(neighborPos);

  Block neightboringBlock = m_loadedChunks.at(neighborChunkPos).GetBlockAt(neightborLocalPos);

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
