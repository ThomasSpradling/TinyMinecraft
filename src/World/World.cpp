#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "World/World.h"
#include "Geometry/geometry.h"
#include "Math/misc.h"
#include "Utils/Logger.h"
#include "Utils/Profiler.h"
#include "Utils/defs.h"
#include "Utils/utils.h"
#include "World/Biome.h"
#include "World/Biome.h"
#include "World/Block.h"
#include "World/Chunk.h"

namespace TinyMinecraft {

  namespace World {

    World::World()
      : m_worldGen(*this)
    {
      // const int NUM_THREADS = 1;
      const int NUM_THREADS = std::thread::hardware_concurrency();

      for (int i = 0; i < NUM_THREADS; ++i) {
        m_workers.emplace_back(&World::DoTasks, this, i);
      }

      Utils::Logger::Message("All {} threads set up.", NUM_THREADS);
    }

    World::~World() {
      m_shouldTerminate.store(true, std::memory_order_release);
      m_nonempty.notify_all();

      for (int i = 0; i < m_workers.size(); ++i) {
        if (m_workers[i].joinable())
          m_workers[i].join();
      }
      Utils::Logger::Message("All threads joined.");

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

    auto World::ComputeBlockRayInteresection(const Geometry::Ray &ray) -> BlockLocation {
      PROFILE_FUNCTION_Misc()

      float distanceTraveled = 0.0f;
      glm::vec3 currentOrigin = ray.origin;
      glm::vec3 lastBlockpos = glm::floor(ray.origin);

      if (GetBlockAt(lastBlockpos).IsSolid()) {
        return std::make_pair(lastBlockpos, Geometry::Face::None);
      }

      constexpr float epsilon = 1e-4f;

      while (distanceTraveled <= ray.maxRayDistance) {
        auto [location, blockPos, face, stepDistance] = GetRayGridInteresction({ currentOrigin, ray.direction });

        distanceTraveled += stepDistance;
        if (distanceTraveled > ray.maxRayDistance)
          break;

        Block block = GetBlockAt(blockPos);
        if (block.IsSolid()) {
          return std::make_pair(blockPos, face);
        }

        lastBlockpos = blockPos;
        currentOrigin = location + ray.direction * epsilon;
      }

      return std::make_pair(lastBlockpos, Geometry::Face::None);
    }

    void World::Update(const glm::vec3 &playerPos) {
      PROFILE_FUNCTION(Chunk)

      m_playerPosition = playerPos;

      constexpr std::array<glm::ivec2, 5> neighborOffsets = {
        glm::ivec2(0, 0), glm::ivec2(1, 0), glm::ivec2(-1, 0), glm::ivec2(0, 1), glm::ivec2(0, -1)
      };

      constexpr int viewRadius = GFX_RENDER_DISTANCE;
      constexpr int loadRadius = viewRadius + 1;

      const glm::ivec2 playerChunkPos = GetChunkPosFromCoords(playerPos);
      std::vector<glm::ivec2> nearbyChunks;
      std::vector<glm::ivec2> chunksToDelete;

      // std::function<bool(const glm::ivec2 &, int)> IsNearby = [&playerChunkPos](const glm::ivec2 &chunkPos, int radius) {
      //   float distance = std::max(std::abs(chunkPos.x - playerChunkPos.x), std::abs(chunkPos.y - playerChunkPos.y));
      //   return distance <= radius;
      // };

      std::function<bool(const glm::ivec2 &, int)> IsNearby = [&playerChunkPos](const glm::ivec2 &chunkPos, int radius) {
        int dx = playerChunkPos.x - chunkPos.x;
        int dz = playerChunkPos.y - chunkPos.y;
        // float distance = std::max(std::abs(dx), std::abs(dz));
        float distance = dx * dx + dz * dz;
        return distance <= radius * radius;
      };

      for (int dz = -loadRadius; dz <= loadRadius; ++dz) {
        for (int dx = -loadRadius; dx <= loadRadius; ++dx) {
          int x = playerChunkPos.x + dx;
          int z = playerChunkPos.y + dz;

          if (IsNearby(glm::ivec2(x, z), loadRadius)) {
            nearbyChunks.push_back(glm::ivec2(x, z));
          }
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
            ScheduleMeshTask(chunk);
          }
          
          continue;
        }

        bool shouldErase = false;

        if (!IsNearby(chunkPos, viewRadius)) {
          if ((!withinLoadRadius && state == ChunkState::Generated) || state == ChunkState::Loaded) {
            // check that deleting this chunk won't prevent proper working of Meshing chunks
            bool hasNearbyIntermediateChunk = false;
            std::ranges::for_each(neighborOffsets, [&](glm::ivec2 offset) {
              glm::ivec2 pos = glm::ivec2(chunkPos.x + offset.x, chunkPos.y + offset.y);
              if (HasChunk(pos) && GetChunkAt(pos)->GetState() == ChunkState::Meshing) {
                hasNearbyIntermediateChunk = true;
              }
            });

            if (!hasNearbyIntermediateChunk) {
              if (chunk->SetState(state, ChunkState::Unloading)) {
                ScheduleUnloadTask(chunk);
              }
              continue;
            }
          }
        }
      }
    }

    void World::SetBlockAt(const glm::vec3 &pos, BlockType type) {
      Block block = GetBlockAt(pos);
      block.SetType(type);
    }

    auto World::GetBlockAt(const glm::vec3 &pos) -> Block {
      
      glm::ivec2 chunkPos = GetChunkPosFromCoords(pos);
      glm::vec3 offsetPos = GetLocalBlockCoords(pos);

      if (!HasChunk(chunkPos)) {
        return BlockType::AIR;
      }

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
      const auto chunk = GetChunkAt(playerChunkPosBefore);
      if (playerChunkPosBefore == playerChunkPosAfter) {
        if (IsChunkLoaded(playerChunkPosBefore)) {
          chunk->SortTranslucentBlocks(after);
          chunk->BufferTranslucentVertices();
        }
      } else {
        for (const auto &offset : directions) {
          if (IsChunkLoaded(playerChunkPosBefore + offset)) {
            chunk->SortTranslucentBlocks(after);
            chunk->BufferTranslucentVertices();
          }
        }
      }
    }

    void World::SubmitTask(std::function<void()> task) {
      {
        std::unique_lock lk(m_taskMutex);
        m_tasks.push(std::move(task));
      }

      m_nonempty.notify_one();
    }

    void World::ScheduleGenerateTask(Chunk *chunk) {

      if (!chunk) {
        Utils::Logger::Error("Cannot generate task for null chunks");
        exit(1);
      }

      SubmitTask([this, chunk]() {    
        const ChunkState state = chunk->GetState();
        if (state != ChunkState::Generating) {
          Utils::Logger::Warning("Chunk {} had incorrect state while generating!", chunk->GetChunkPos());
          return;
        }

        m_worldGen.GenerateTerrainChunk(chunk);

        chunk->SetState(ChunkState::Generating, ChunkState::Generated);
      });
    }

    void World::ScheduleMeshTask(Chunk *chunk) {
      if (!chunk) {
        Utils::Logger::Error("Cannot mesh task for null chunks");
        exit(1);
      }

      SubmitTask([this, chunk]() {
        const ChunkState state = chunk->GetState();

        if (state != ChunkState::Meshing) {
          Utils::Logger::Warning("Chunk {} had incorrect state while meshing!", chunk->GetChunkPos());
          return;
        }

        chunk->UpdateMesh();
        chunk->UpdateTranslucentMesh(m_playerPosition);
        chunk->SetDirty(true);
        chunk->SetTranslucentDirty(true);

        chunk->SetState(ChunkState::Meshing, ChunkState::Loaded);
      });
    }

    void World::ScheduleUnloadTask(Chunk *chunk) {
      if (!chunk) {
        Utils::Logger::Error("Cannot unload task for null chunks");
        exit(1);
      }

      SubmitTask([this, chunk]() {
        const ChunkState state = chunk->GetState();

        if (state != ChunkState::Unloading) {
          Utils::Logger::Warning("Chunk {} had incorrect state while unloading!", chunk->GetChunkPos());
          return;
        }
        
        chunk->SetShouldClear(true);
        chunk->ClearBlocks();
        chunk->SetState(ChunkState::Unloading, ChunkState::Empty);
      });
    }


    void World::DoTasks(int i) {
      Utils::SetThreadName("chunk_worker " + std::to_string(i));

      while(true) {
        std::function<void()> task;
        {
          std::unique_lock lk(m_taskMutex);
          m_nonempty.wait(lk, [&]() { return !m_tasks.empty() || m_shouldTerminate; });

          if (m_shouldTerminate) {
#ifdef __DEBUG__
            Utils::Logger::Debug("Terminating.");
#endif
            return;
          }

          task = std::move(m_tasks.front());
          m_tasks.pop();
        }

        task();
      }
    }

  }

}
