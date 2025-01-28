#ifndef CHUNK_H_
#define CHUNK_H_

#include <vector>

#include "Graphics/BufferObject.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/VertexArray.h"
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
  std::vector<GLfloat> vertices;
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

  void Render(Graphics::Texture &blockAtlasTexture, Graphics::Shader &blockShader, const glm::vec3 &playerPos);
  void RenderTranslucent(Graphics::Texture &blockAtlasTexture, Graphics::Shader &waterShader, const glm::vec3 &playerPos);

  void UpdateMesh();
  void UpdateTranslucentMesh(const glm::vec3 &playerPos);
  void SortTranslucentBlocks(const glm::vec3 &playerPos);

  auto GetBlockAt(int x, int y, int z) -> Block &;
  auto GetBlockAt(const glm::ivec3 &pos) -> Block &;

  void SetBlockAt(int x, int y, int z, const Block &block);
  void SetBlockAt(const glm::ivec3 &pos, const Block &block);

  [[nodiscard]] auto HasTranslucentBlocks() const -> bool;
  
  auto GetVertexCount() -> GLsizei;

  void SetHidden(bool value);
  [[nodiscard]] auto IsHidden() const -> bool;

  [[nodiscard]] auto IsDirty() const -> bool;
  void SetDirty(bool value);

  void SetState(ChunkState value);
  [[nodiscard]] auto GetState() const -> ChunkState;

  [[nodiscard]] auto GetChunkPos() const -> glm::ivec2;

  auto GetSurfaceHeight(int x, int z) -> int;
  
private:
  World &m_world;

  static constexpr int BLOCK_COUNT = CHUNK_WIDTH * CHUNK_LENGTH * CHUNK_HEIGHT;

  std::array<Block, BLOCK_COUNT> m_blocks;
  std::vector<FaceGeometry> m_translucentFaces;

  Graphics::VertexArray m_opaqueVAO, m_transVAO;
  Graphics::BufferObject m_opaqueVBO, m_transVBO, m_opaqueEBO, m_transEBO;

  glm::ivec2 m_chunkPos;

  bool m_hidden = true;
  ChunkState m_state = ChunkState::Unloaded;

  bool m_dirty = true;

  bool m_hasTranslucentBlocks = false;

  size_t m_opaqueVertexCount = 0, m_transVertexCount = 0;

  void InitializeOpaque();
  void InitializeTranslucent();
};

}

#endif // CHUNK_H_
