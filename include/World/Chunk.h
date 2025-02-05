#ifndef CHUNK_H_
#define CHUNK_H_

#include <algorithm>
#include <array>
#include <vector>
#include <memory>
#include <atomic>

#include "Geometry/Mesh.h"
#include "Utils/Profiler.h"
#include "Utils/defs.h"
#include "World/Block.h"
#include "Graphics/gfx.h"

namespace TinyMinecraft {

  namespace World {

    class World;

    enum class ChunkState : uint8_t {
      Empty = 0,
      Unloading,
      Generating,
      Generated,
      Meshing,
      Loaded,
    };

    struct FaceGeometry {
      glm::vec3 pos;
      std::vector<Geometry::MeshVertex> vertices;
      std::vector<GLuint> indices;
      float distanceToPlayer;
    };

    class Chunk {
    public:
      Chunk(World &world, const glm::ivec2 &chunkPos);

      ~Chunk() = default;
      Chunk(const Chunk &other) = delete;
      auto operator=(const Chunk &other) -> Chunk & = delete;
      Chunk(Chunk &&other) noexcept;
      auto operator=(Chunk &&other) noexcept -> Chunk &;

      void UpdateMesh();
      void BufferVertices();
      void BufferTranslucentVertices();
      void UpdateTranslucentMesh(const glm::vec3 &playerPos);
      void SortTranslucentBlocks(const glm::vec3 &playerPos);

      void ReserveBlocks() {
        m_data.blocks.resize(m_data.BLOCK_COUNT, BlockType::AIR);
      }
      void ClearBlocks() { m_data.blocks.clear(); }

      inline auto GetBlockAt(int x, int y, int z) -> Block & {
        return m_data.blocks.at(CHUNK_LENGTH * CHUNK_HEIGHT * x + CHUNK_LENGTH * y + z);
      }
      auto GetBlockAt(const glm::ivec3 &pos) -> Block & { return GetBlockAt(pos.x, pos.y, pos.z); }

      inline void SetBlockAt(int x, int y, int z, const Block &block) {
        m_data.blocks.at(CHUNK_LENGTH * CHUNK_HEIGHT * x + CHUNK_LENGTH * y + z) = block;
      }
      void SetBlockAt(const glm::ivec3 &pos, const Block &block) { SetBlockAt(pos.x, pos.y, pos.z, block); }

      auto GetSurfaceHeight(int x, int z) -> int;

      inline auto GetGlobalCoords(const glm::vec3 &pos) const -> glm::vec3 {
        return glm::vec3(m_chunkPos.x * CHUNK_WIDTH + pos.x, pos.y, m_chunkPos.y * CHUNK_LENGTH + pos.z);
      }

      [[nodiscard]] inline auto GetMesh() const -> Geometry::Mesh & { return *m_opaqueMesh; }
      [[nodiscard]] inline auto GetTranslucentMesh() const -> Geometry::Mesh & { return *m_translucentMesh; }
      [[nodiscard]] inline auto HasTranslucentBlocks() const -> bool { return m_hasTranslucentBlocks; }
      
      [[nodiscard]] inline auto IsHidden() const -> bool { return m_hidden; }
      inline void SetHidden(bool value) { m_hidden = value; }

      [[nodiscard]] inline auto IsDirty() const -> bool { return m_dirty.load(std::memory_order_acquire); }
      inline void SetDirty(bool value) { m_dirty.store(value, std::memory_order_release); }

      [[nodiscard]] inline auto IsTranslucentDirty() const -> bool { return m_translucentDirty.load(std::memory_order_acquire); }
      inline void SetTranslucentDirty(bool value) { m_translucentDirty.store(value, std::memory_order_release); }

      [[nodiscard]] inline auto GetState() const -> ChunkState {
        return m_state.load(std::memory_order_acquire);
      }

      auto SetState(ChunkState expected, ChunkState desired) -> bool {
        return m_state.compare_exchange_strong(expected, desired);
      }

      [[nodiscard]] auto GetChunkPos() const -> glm::ivec2 { return m_chunkPos; }
      
    private:
      World &m_world;
      glm::ivec2 m_chunkPos;

      struct ChunkData {
        static constexpr int BLOCK_COUNT = CHUNK_WIDTH * CHUNK_LENGTH * CHUNK_HEIGHT;

        std::vector<Block> blocks;
        std::vector<FaceGeometry> translucentFaces;
      } m_data;

      std::unique_ptr<Geometry::Mesh> m_opaqueMesh, m_translucentMesh;
      std::vector<Geometry::MeshVertex> m_opaqueVertices, m_translucentVertices;
      std::vector<GLuint> m_opaqueIndices, m_translucentIndices;

      bool m_hidden = true;
      std::atomic<ChunkState> m_state { ChunkState::Empty };
      std::atomic<bool> m_dirty = false;
      std::atomic<bool> m_translucentDirty = false;

      bool m_hasTranslucentBlocks = false;
    };

  }

}

#endif // CHUNK_H_
