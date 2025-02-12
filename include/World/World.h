#ifndef WORLD_H_
#define WORLD_H_

#include "Geometry/geometry.h"
#include "Utils/Logger.h"
#include "Utils/mathgl.h"
#include "World/BlockType.h"
#include "World/Chunk.h"
#include "World/WorldGeneration.h"
#include <functional>
#include <queue>
#include <tbb/concurrent_unordered_map.h>
#include <memory>
#include <thread>

namespace TinyMinecraft {

  namespace World {
    using ChunkMap = tbb::concurrent_unordered_map<glm::ivec2, std::shared_ptr<Chunk>, Utils::IVec2Hash>;
    using BlockLocation = std::tuple<glm::vec3, Geometry::Face, BlockType>;

    class World {
    public:
      World();
      ~World();

      auto GetTemperature(int x, int z) -> double;
      auto GetHumidity(int x, int z) -> double;
      auto GetContinentalness(int x, int z) -> double;
      auto GetErosion(int x, int z) -> double;
      auto GetRidges(int x, int z) -> double;

      auto ComputeBlockRayInteresection(const Geometry::Ray &ray) -> BlockLocation;

      auto GetBiome(int x, int z) -> BiomeType;

      void Update(const glm::vec3 &playerPos);
      void RefreshChunkAt(const glm::vec3 &pos);

      void BreakBlock(const glm::vec3 &pos);
      void SetBlockAt(const glm::vec3 &pos, BlockType type);

      [[nodiscard]] inline auto GetPlayerPosition() -> glm::vec3 { return m_playerPosition; }
      
      [[nodiscard]] inline auto HasChunk(const glm::ivec2 &chunkPos) const -> bool { return m_chunks.contains(chunkPos); }
      [[nodiscard]] inline auto IsChunkLoaded(const glm::ivec2 &chunkPos) const -> bool {
        if (!HasChunk(chunkPos)) return false;
        return m_chunks.at(chunkPos)->GetState() == ChunkState::Loaded;
      }
      [[nodiscard]] inline auto IsChunkEmpty(const glm::ivec2 &chunkPos) const -> bool {
        if (!HasChunk(chunkPos)) return false;
        return m_chunks.at(chunkPos)->GetState() == ChunkState::Empty;
      }
      [[nodiscard]] inline auto GetChunks() const -> const ChunkMap & {
        return m_chunks;
      }
      [[nodiscard]] inline auto GetChunkAt(int x, int z) const -> const std::shared_ptr<Chunk> & {
        return GetChunkAt(glm::ivec2(x, z));
      }
      [[nodiscard]] inline auto GetChunkAt(const glm::ivec2 &chunkPos) const -> const std::shared_ptr<Chunk> & {
        if (!HasChunk(chunkPos)) {
          Utils::Logger::Error("Cannot get chunk at {}", chunkPos);
        }
        return m_chunks.at(chunkPos);
      }

      
      [[nodiscard]] auto GetBlockAt(const glm::vec3 &pos) -> BlockType;
      [[nodiscard]] auto HasBlock(const glm::vec3 &pos) -> bool;
      
      [[nodiscard]] auto GetChunkPosFromCoords(const glm::vec3 &pos) -> glm::ivec2;
      [[nodiscard]] auto GetLocalBlockCoords(const glm::vec3 &pos) -> glm::vec3;

      void HandlePlayerMovement(const glm::vec3 &before, const glm::vec3 &after);

    private:
      const unsigned int seed = 3782;

      std::atomic<bool> m_shouldTerminate;
      std::condition_variable m_nonempty;
      std::mutex m_taskMutex;

      std::queue<std::function<void()>> m_tasks;
      std::vector<std::thread> m_workers;

      glm::vec3 m_playerPosition;

      ChunkMap m_chunks;
      WorldGeneration m_worldGen;

      void SubmitTask(std::function<void()> task);
      void ScheduleGenerateTask(Chunk *chunk);
      void ScheduleUnloadTask(Chunk *chunk);
      void ScheduleMeshTask(Chunk *chunk);
      
      void DoTasks(int i);
    };

  }

}

#endif // WORLD_H_
