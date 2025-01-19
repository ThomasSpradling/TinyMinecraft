#ifndef CHUNK_H_
#define CHUNK_H_

#include <memory>

#include "Utils/defs.h"
#include "World/Block.h"
#include "World/BlockFace.h"
#include "Graphics/gfx.h"

namespace World {

class World;

class Chunk {
public:
  Chunk(World &world, glm::ivec2 &chunkPos) : world(world), chunkPos(chunkPos) {}
  ~Chunk() = default;

  void Init();

  void UpdateMesh();

  Block &GetBlockAt(GLuint x, GLuint y, GLuint z);
  Block &GetBlockAt(const glm::vec3 &pos);

  void SetBlockAt(GLuint x, GLuint y, GLuint z, const Block &block);
  void SetBlockAt(const glm::vec3 &pos, const Block &block);
  
  GLuint GetMeshVAO() const;
  GLsizei GetVertexCount();

  void SetDirty(bool value);
  bool IsDirty();

  void SetLoaded(bool value);
  bool IsLoaded();

  glm::ivec2 &GetChunkPos();

private:
  World &world;

  Block blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_LENGTH];
  GLuint VAO, VBO, EBO;

  glm::ivec2 chunkPos;

  bool dirty = true;
  bool loaded = true;

  int vertexCount = 0;
};

}

#endif // CHUNK_H_
