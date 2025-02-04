#ifndef WORLD_H_
#define WORLD_H_

#include "Geometry/geometry.h"
#include "Utils/Logger.h"
#include "Utils/mathgl.h"
#include "World/Chunk.h"
#include "World/WorldGeneration.h"
#include <condition_variable>
#include <functional>
#include <memory>
#include <queue>
#include <thread>

namespace TinyMinecraft {

  namespace World {

    class World {
    public:
      World();
      ~World();

      auto GetTemperature(int x, int z) -> double;
      auto GetHumidity(int x, int z) -> double;
      auto GetContinentalness(int x, int z) -> double;
      auto GetErosion(int x, int z) -> double;
      auto GetRidges(int x, int z) -> double;

      auto GetBiome(int x, int z) -> BiomeType;

      void Update(const glm::vec3 &playerPos);
      auto IsFaceVisible(const Block &block, Geometry::Face face, const glm::vec3 &pos) -> bool;

      inline auto HasChunk(const glm::ivec2 &chunkPos) const -> bool { return m_chunks.contains(chunkPos); }
      inline auto IsChunkLoaded(const glm::ivec2 &chunkPos) const -> bool {
        if (!HasChunk(chunkPos)) return false;
        return m_chunks.at(chunkPos)->GetState() == ChunkState::Loaded;
      }
      inline auto GetChunks() const -> const std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>, Utils::IVec2Hash> & {
        return m_chunks;
      }
      inline auto GetChunkAt(int x, int z) const -> const std::unique_ptr<Chunk> & {
        return GetChunkAt(glm::ivec2(x, z));
      }
      inline auto GetChunkAt(const glm::ivec2 &chunkPos)  const -> const std::unique_ptr<Chunk> & {
        if (!HasChunk(chunkPos)) {
          Utils::g_logger.Error("Cannot get chunk at {}", chunkPos);
        }
        return m_chunks.at(chunkPos);
      }

      void BreakBlock(const glm::vec3 &pos);
      void SetBlockAt(const glm::vec3 &pos, BlockType type);
      
      auto GetBlockAt(const glm::vec3 &pos) -> Block &;
      auto HasBlock(const glm::vec3 &pos) -> bool;
      
      auto GetChunkPosFromCoords(const glm::vec3 &pos) -> glm::ivec2;
      auto GetLocalBlockCoords(const glm::vec3 &pos) -> glm::vec3;

      void HandlePlayerMovement(const glm::vec3 &before, const glm::vec3 &after);

    private:
      const unsigned int seed = 3782;

      std::atomic<bool> m_shouldTerminate;
      std::condition_variable m_nonempty;
      std::mutex m_taskMutex;

      std::queue<std::function<void()>> m_tasks;
      std::vector<std::thread> m_workers;

      std::mutex m_chunkMutex;
      std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>, Utils::IVec2Hash> m_chunks;
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
