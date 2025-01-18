#ifndef CHUNK_H_
#define CHUNK_H_

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 256
#define CHUNK_LENGTH 16

#include "World/Block.h"
#include "World/BlockFace.h"
#include "Graphics/gfx.h"

namespace World {

class Chunk {
public:
  Chunk(glm::vec2 offset = glm::vec2(0.0f)) : offset(offset) {};
  ~Chunk() = default;

  void Init();

  void UpdateMesh();

  Block GetBlockAt(GLuint x, GLuint y, GLuint z);
  Block GetBlockAt(const glm::vec3 &pos);

  void SetBlockAt(GLuint x, GLuint y, GLuint z, const Block &block);
  void SetBlockAt(const glm::vec3 &pos, const Block &block);
  
  GLuint GetMeshVAO() const;
  GLsizei GetVertexCount();

  void SetDirty(bool value);
  bool IsDirty();

  glm::vec2 GetOffset();

private:
  Block blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_LENGTH];
  GLuint VAO, VBO, EBO;

  glm::vec2 offset;

  bool dirty = true;

  int vertexCount = 0;

  bool FaceIsVisible(BlockFace face, const glm::vec3 &pos);
};

}

#endif // CHUNK_H_
