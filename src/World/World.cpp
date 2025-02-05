#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "World/World.h"
#include "Geometry/geometry.h"
#include "Utils/Logger.h"
#include "Utils/Profiler.h"
#include "Utils/defs.h"
#include "Utils/utils.h"
#include "World/Biome.h"
#include "World/Block.h"
#include "World/Chunk.h"

namespace TinyMinecraft {

  namespace World {

    World::World()
      : m_worldGen(*this)
    {
      constexpr int NUM_THREADS = 10;

      for (int i = 0; i < NUM_THREADS; ++i) {
        m_workers.emplace_back(&World::DoTasks, this, i);
      }
      Utils::g_logger.Debug("All threads made...");
    }

    World::~World() {
      m_shouldTerminate.store(true, std::memory_order_release);

      for (int i = 0; i < m_workers.size(); ++i) {
        if (m_workers[i].joinable())
          m_workers[i].join();
      }

      m_workers.clear();
    }

    auto World::GetTemperature(int x, int z) -> double {
      // const double temperatureScale = 1;

      // double temperature = Math::NoiseManager::GetImprovedSimplexNoise(
      //   Math::Noise::Temperature,
      //   glm::vec2(x * temperatureScale, z * temperatureScale));

      // temperature = Utils::ScaleValue(0.3, 0.7, 0.1, 0.9, temperature);
      // temperature = std::clamp(temperature, 0.0, 1.0);

      // return tsemperature;

      return 0;
    }

    auto World::GetHumidity(int x, int z) -> double {
      // const double humidityScale = 0.05;

      // double humidity = Math::NoiseManager::GetImprovedSimplexNoise(
      //   Math::Noise::Humidity,
      //   glm::vec2(x * humidityScale, z * humidityScale));
      // return humidity;
      return 0;
    }

    auto World::GetContinentalness(int x, int z) -> double {
      // double continentalness = Math::NoiseManager::GetImprovedSimplexNoise(
      //   Math::Noise::Continentalness,
      //   glm::vec2(x, z));

      // return continentalness;
      return 0;
    }

    auto World::GetRidges(int x, int z) -> double {
      // double ridges = Math::NoiseManager::GetImprovedSimplexNoise(
      //   Math::Noise::Ridges,
      //   glm::vec2(x, z));

      // return 1 - ridges;
      return 0;
    }

    auto World::GetErosion(int x, int z) -> double {
      // double erosion = Math::NoiseManager::GetImprovedSimplexNoise(
      //   Math::Noise::Erosion,
      //   glm::vec2(x, z));

      // return erosion;
      return 0;
    }

    auto World::GetBiome(int x, int z) -> BiomeType {
      return m_worldGen.SelectBiomes(GetTemperature(x, z), GetHumidity(x, z)).first->GetType();
    }

    void World::Update(const glm::vec3 &playerPos) {
      PROFILE_FUNCTION(Chunk)
      constexpr std::array<glm::ivec2, 5> neighborOffsets = {
        glm::ivec2(0, 0), glm::ivec2(1, 0), glm::ivec2(-1, 0), glm::ivec2(0, 1), glm::ivec2(0, -1)
      };

      constexpr int viewRadius = GFX_RENDER_DISTANCE;
      constexpr int loadRadius = viewRadius + 1;

      const glm::ivec2 playerChunkPos = GetChunkPosFromCoords(playerPos);
      std::vector<glm::ivec2> nearbyChunks;
      std::vector<glm::ivec2> chunksToDelete;

      std::function<bool(const glm::ivec2 &, int)> IsNearby = [&playerChunkPos](const glm::ivec2 &chunkPos, int radius) {
        float distance = std::max(std::abs(chunkPos.x - playerChunkPos.x), std::abs(chunkPos.y - playerChunkPos.y));
        return distance <= radius;
      };

      for (int z = -loadRadius; z <= loadRadius; ++z) {
        for (int x = -loadRadius; x <= loadRadius; ++x) {
          int dx = playerChunkPos.x + x;
          int dz = playerChunkPos.y + z;

          nearbyChunks.push_back(glm::ivec2(dx, dz));
        }
      }

      for (const glm::ivec2 &chunkPos : nearbyChunks) {
        if (!HasChunk(chunkPos)) {
          m_chunks[chunkPos] = std::make_unique<Chunk>(*this, chunkPos);
        }
      }

      for (auto it = m_chunks.begin(); it != m_chunks.end(); ++it) {
        Chunk *chunk = it->second.get();
        const glm::ivec2 chunkPos = it->first;
        const ChunkState state = chunk->GetState();

        const int withinLoadRadius = IsNearby(chunkPos, loadRadius);

        if (withinLoadRadius) {
          if (state == ChunkState::Empty && chunk->SetState(ChunkState::Empty, ChunkState::Generating)) {
            Utils::g_logger.Debug("Scheduling generate task: {}", chunkPos);
            ScheduleGenerateTask(chunk);
            continue;
          }

          // check that this chunk and all its neighbors are generated before attempting to mesh

          // needs to atomically assure this stuff....
          bool neighborsGenerated = true;
          std::ranges::for_each(neighborOffsets, [&](glm::ivec2 offset) {
            glm::ivec2 pos = glm::ivec2(chunkPos.x + offset.x, chunkPos.y + offset.y);
            if (!HasChunk(pos) || GetChunkAt(pos)->GetState() < ChunkState::Generated) {
              neighborsGenerated = false;
            }
          });

          if (neighborsGenerated && chunk->SetState(ChunkState::Generated, ChunkState::Meshing)) {
            Utils::g_logger.Debug("Scheduling meshing task: {}", chunkPos);
            ScheduleMeshTask(chunk);
          }
          
          continue;
        }

        bool shouldErase = false;

        if (!IsNearby(chunkPos, viewRadius)) {
          // Utils::g_logger.Debug("Chunk out of range: {}. It has state {}", chunkPos, static_cast<int>(state));

          // if ((!withinLoadRadius && state == ChunkState::Generated) || state == ChunkState::Loaded) {
          //   // check that deleting this chunk won't prevent proper working of Meshing chunks
            
          //   std::unique_lock lk(m_chunkMutex);
          //   {
          //     bool hasNearbyIntermediateChunk = false;
          //     std::ranges::for_each(neighborOffsets, [&](glm::ivec2 offset) {
          //       glm::ivec2 pos = glm::ivec2(chunkPos.x + offset.x, chunkPos.y + offset.y);
          //       if (HasChunk(pos) && GetChunkAt(pos)->GetState() == ChunkState::Meshing) {
          //         hasNearbyIntermediateChunk = true;
          //       }
          //     });

          //     if (!hasNearbyIntermediateChunk) {
          //       // if (chunk->SetState(ChunkState::Generated, ChunkState::Unloading)) {
          //       //   Utils::g_logger.Debug("Scheduling unloading task: {}", chunkPos);
          //       //   ScheduleUnloadTask(chunk);
          //       // }
          //       // continue;
          //     }
          //   }
          // }
        }
      }

      // compute which chunks are nearby right now
      // for each 

      // const glm::ivec2 playerChunkPos = GetChunkPosFromCoords(playerPos);
      // constexpr int viewRadius = GFX_RENDER_DISTANCE;
      // constexpr int loadRadius = viewRadius + 1;
      // std::vector<glm::ivec2> chunksToUnload;

      // // unload all nearby chunks

      // for (auto &[chunkPos, chunk] : m_loadedChunks) {
      //   // if not nearby

      //   int dx = chunkPos.x - playerChunkPos.x;
      //   int dz = chunkPos.y - playerChunkPos.y;

      //   int distanceSquared = dx * dx + dz * dz;

      //   if (distanceSquared > loadRadius * loadRadius) {
      //     chunksToUnload.push_back(chunkPos);
      //   }
      // }

      // for (const auto &chunkPos : chunksToUnload) {
      //   UnloadChunk(m_loadedChunks.at(chunkPos));
      // }

      // for (int dz = -loadRadius; dz <= loadRadius; ++dz) {
      //   for (int dx = -loadRadius; dx <= loadRadius; ++dx) {
      //     int distanceSquared = dx * dx + dz * dz;

      //     if (distanceSquared <= loadRadius * loadRadius) {
      //       glm::ivec2 chunkPos { playerChunkPos.x + dx, playerChunkPos.y + dz };

      //       if (!m_loadedChunks.contains(chunkPos)) {
      //         LoadChunk(chunkPos.x, chunkPos.y);
      //       }
      //     }
      //   }
      // }

      // for (int dz = -loadRadius; dz <= loadRadius; ++dz) {
      //   for (int dx = -loadRadius; dx <= loadRadius; ++dx) {
      //     int distanceSquared = dx * dx + dz * dz;

      //     if (distanceSquared <= loadRadius * loadRadius) {
      //       glm::ivec2 chunkPos { playerChunkPos.x + dx, playerChunkPos.y + dz };

      //       m_worldGen.LoadUnloadedBlocks(GetChunkAt(chunkPos));
      //     }
      //   }
      // }

      // // Figure out closet dirty chunk in range

      // int shortestDistance = std::numeric_limits<int>::max();
      // Chunk *closestDirtyChunk = nullptr;

      // for (auto &[chunkPos, chunk] : m_loadedChunks) {
      //   int dx = chunkPos.x - playerChunkPos.x;
      //   int dz = chunkPos.y - playerChunkPos.y;

      //   int distanceSquared = dx * dx + dz * dz;

      //   if (distanceSquared <= viewRadius * viewRadius && chunk.IsDirty()) {
      //     if (distanceSquared < shortestDistance) {
      //       closestDirtyChunk = &chunk;
      //       shortestDistance = distanceSquared;
      //     }
      //   }
      // }

      // // if there is a closet dirty chunk in range, update its mesh

      // if (closestDirtyChunk) {
      //   closestDirtyChunk->UpdateMesh();
      //   closestDirtyChunk->UpdateTranslucentMesh(playerPos);
      //   closestDirtyChunk->SetDirty(false);
      // }
    }

    void World::SetBlockAt(const glm::vec3 &pos, BlockType type) {
      Block &block = GetBlockAt(pos);
      block.SetType(type);
    }

    auto World::GetBlockAt(const glm::vec3 &pos) -> Block & {
      glm::ivec2 chunkPos = GetChunkPosFromCoords(pos);
      glm::vec3 offsetPos = GetLocalBlockCoords(pos);

      return GetChunkAt(chunkPos)->GetBlockAt(offsetPos);
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
          GetChunkAt(playerChunkPosBefore)->SortTranslucentBlocks(after);
        }
      } else {
        for (const auto &offset : directions) {
          if (IsChunkLoaded(playerChunkPosBefore + offset)) {
            GetChunkAt(playerChunkPosBefore + offset)->SortTranslucentBlocks(after);
          }
        }
      }
    }


    // thread-safe since all updates are atomic, making this read-only section OK
    auto World::IsFaceVisible(const Block &block, Geometry::Face face, const glm::vec3 &pos) -> bool {
      glm::vec3 neighborPos = pos + Geometry::GetNormal(face);
      glm::ivec2 neighborChunkPos = GetChunkPosFromCoords(neighborPos);

      // if not in a chunk
      if (neighborPos.y >= CHUNK_HEIGHT || neighborPos.y < 0 || !HasChunk(neighborChunkPos)) {
        return true;
      }

      std::shared_ptr<Chunk> neighborChunk = GetChunkAt(neighborChunkPos);
      if (!neighborChunk || neighborChunk->GetState() < ChunkState::Generated) {
        return true;
      }
      
      glm::vec3 neightborLocalPos = GetLocalBlockCoords(neighborPos);

      Block neighboringBlock = neighborChunk->GetBlockAt(neightborLocalPos);

      if (neighboringBlock.GetType() == BlockType::AIR) {
        return true;
      }

      if (!block.IsTransparent()) {
        return neighboringBlock.IsTransparent();
      }

      if (block.IsTransparent()) {
        if (neighboringBlock.IsTransparent()) {
          return neighboringBlock.GetType() != block.GetType();
        }
      }

      return false;
    }

    void World::SubmitTask(std::function<void()> task) {
      m_tasks.enqueue(std::move(task));
    }

    void World::ScheduleGenerateTask(Chunk *chunk) {

      if (!chunk) {
        Utils::g_logger.Error("Cannot generate task for null chunks");
        exit(1);
      }

      SubmitTask([this, chunk]() {    
        const ChunkState state = chunk->GetState();
        if (state != ChunkState::Generating) {
          Utils::g_logger.Warning("Chunk {} had incorrect state while generating!", chunk->GetChunkPos());
          return;
        }

        Utils::g_logger.Debug("Doing generating task. {}", chunk->GetChunkPos());

        m_worldGen.GenerateTerrainChunk(chunk);

        chunk->SetState(ChunkState::Generating, ChunkState::Generated);
      });
    }

    void World::ScheduleMeshTask(Chunk *chunk) {
      if (!chunk) {
        Utils::g_logger.Error("Cannot mesh task for null chunks");
        exit(1);
      }

      SubmitTask([this, chunk]() {
        const ChunkState state = chunk->GetState();

        if (state != ChunkState::Meshing) {
          Utils::g_logger.Warning("Chunk {} had incorrect state while meshing!", chunk->GetChunkPos());
          return;
        }

        Utils::g_logger.Debug("Doing meshing task. {}", chunk->GetChunkPos());

        chunk->UpdateMesh();
        chunk->SetDirty(true);

        chunk->SetState(ChunkState::Meshing, ChunkState::Loaded);
      });
    }

    void World::ScheduleUnloadTask(Chunk *chunk) {
      if (!chunk) {
        Utils::g_logger.Error("Cannot unload task for null chunks");
        exit(1);
      }

      SubmitTask([this, chunk]() {
        const ChunkState state = chunk->GetState();

        if (state != ChunkState::Unloading) {
          Utils::g_logger.Warning("Chunk {} had incorrect state while unloading!", chunk->GetChunkPos());
          return;
        }

        Utils::g_logger.Debug("Doing unloading task. {}", chunk->GetChunkPos());


        chunk->SetState(ChunkState::Unloading, ChunkState::Empty);

        // std::lock_guard<std::mutex> lk(m_chunkMutex);
        // glm::ivec2 chunkPos = chunk->GetChunkPos();
        // if (HasChunk(chunkPos)) {
        //   m_chunks.erase(chunkPos);
        // }
      });
    }


    void World::DoTasks(int i) {
      Utils::SetThreadName("chunk_worker " + std::to_string(i));

      std::function<void()> task;
      while(!m_shouldTerminate.load(std::memory_order_acquire)) {
        if (m_tasks.try_dequeue(task)) {
          task();
        } else {
          std::this_thread::yield();
        }
      }
    }

  }

}
