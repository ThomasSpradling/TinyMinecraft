#ifndef CHUNK_H_
#define CHUNK_H_

#include <vector>

#include "Geometry/Mesh.h"
#include "Utils/defs.h"
#include "World/Block.h"
#include "Graphics/gfx.h"

namespace World {

class World;

enum class ChunkState : uint8_t {
  Unloaded = 0,
  Loaded,
  Loading
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
  void UpdateTranslucentMesh(const glm::vec3 &playerPos);
  void SortTranslucentBlocks(const glm::vec3 &playerPos);

  auto GetBlockAt(int x, int y, int z) -> Block &;
  auto GetBlockAt(const glm::ivec3 &pos) -> Block &;

  void SetBlockAt(int x, int y, int z, const Block &block);
  void SetBlockAt(const glm::ivec3 &pos, const Block &block);

  auto GetSurfaceHeight(int x, int z) -> int;

  inline auto GetGlobalCoords(const glm::vec3 &pos) const -> glm::vec3 {
    return glm::vec3(m_chunkPos.x * CHUNK_WIDTH + pos.x, pos.y, m_chunkPos.y * CHUNK_LENGTH + pos.z);
  }

  [[nodiscard]] inline auto GetMesh() const -> Geometry::Mesh & { return *m_opaqueMesh; }
  [[nodiscard]] inline auto GetTranslucentMesh() const -> Geometry::Mesh & { return *m_translucentMesh; }
  [[nodiscard]] inline auto HasTranslucentBlocks() const -> bool { return m_hasTranslucentBlocks; }
  
  [[nodiscard]] inline auto IsHidden() const -> bool { return m_hidden; }
  inline void SetHidden(bool value) { m_hidden = value; }

  [[nodiscard]] inline auto IsDirty() const -> bool { return m_dirty; }
  inline void SetDirty(bool value) { m_dirty = value; }

  [[nodiscard]] auto GetState() const -> ChunkState { return m_state; }
  void SetState(ChunkState value) { m_state = value; }

  [[nodiscard]] auto GetChunkPos() const -> glm::ivec2 { return m_chunkPos; }
  
private:
  World &m_world;
  glm::ivec2 m_chunkPos;

  struct ChunkData {
    static constexpr int BLOCK_COUNT = CHUNK_WIDTH * CHUNK_LENGTH * CHUNK_HEIGHT;

    std::array<Block, BLOCK_COUNT> blocks;
    std::vector<FaceGeometry> translucentFaces;
  } m_data;

  std::unique_ptr<Geometry::Mesh> m_opaqueMesh, m_translucentMesh;

  bool m_hidden = true;
  ChunkState m_state = ChunkState::Unloaded;
  bool m_dirty = true;

  bool m_hasTranslucentBlocks = false;
};

}

#endif // CHUNK_H_
