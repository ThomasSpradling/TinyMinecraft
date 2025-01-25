#ifndef CHUNK_H_
#define CHUNK_H_

#include <memory>

#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Utils/defs.h"
#include "World/Block.h"
#include "World/BlockFace.h"
#include "Graphics/gfx.h"

namespace World {

class World;

enum ChunkState {
  Unloaded = 0,
  Loaded,
  Loading
};

class Chunk {
public:
  Chunk(World &world, const glm::ivec2 &chunkPos) : world(world), chunkPos(chunkPos) {}
  ~Chunk() = default;

  void Initialize();

  void Render(Graphics::Texture &blockAtlasTexture, GLuint depthMap, Graphics::Shader &blockShader, const glm::vec3 &playerPos);
  void UpdateMesh();

  Block &GetBlockAt(GLuint x, GLuint y, GLuint z);
  Block &GetBlockAt(const glm::vec3 &pos);

  void SetBlockAt(GLuint x, GLuint y, GLuint z, const Block &block);
  void SetBlockAt(const glm::vec3 &pos, const Block &block);
  
  GLuint GetMeshVAO() const;
  GLsizei GetVertexCount();

  void SetHidden(bool value);
  bool IsHidden();

  bool IsDirty() { return dirty; }
  void SetDirty(bool value) { dirty = value; }

  void SetState(ChunkState value);
  ChunkState GetState();

  glm::ivec2 &GetChunkPos();

private:
  [[maybe_unused]]World &world;

  Block blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_LENGTH];
  GLuint VAO, VBO, EBO;

  glm::ivec2 chunkPos;

  bool hidden = true;
  ChunkState state = ChunkState::Unloaded;

  bool dirty = true;

  int vertexCount = 0;
};

}

#endif // CHUNK_H_
