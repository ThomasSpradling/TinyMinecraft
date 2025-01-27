#ifndef CHUNK_H_
#define CHUNK_H_

#include <memory>
#include <vector>

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

struct FaceGeometry {
  glm::vec3 pos;
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;
  float distanceToPlayer;
};

class Chunk {
public:
  Chunk(World &world, const glm::ivec2 &chunkPos) : world(world), chunkPos(chunkPos) {}
  ~Chunk() = default;

  void Initialize();

  void Render(Graphics::Texture &blockAtlasTexture, GLuint depthMap, Graphics::Shader &blockShader, const glm::vec3 &playerPos);
  void RenderTranslucent(Graphics::Texture &blockAtlasTexture, GLuint depthMap, Graphics::Shader &waterShader, const glm::vec3 &playerPos);

  void UpdateMesh();
  void UpdateTranslucentMesh(const glm::vec3 &playerPos);
  void SortTranslucentBlocks(const glm::vec3 &playerPos);

  Block &GetBlockAt(GLuint x, GLuint y, GLuint z);
  Block &GetBlockAt(const glm::vec3 &pos);

  void SetBlockAt(GLuint x, GLuint y, GLuint z, const Block &block);
  void SetBlockAt(const glm::vec3 &pos, const Block &block);

  bool HasTranslucentBlocks();
  
  GLsizei GetVertexCount();

  void SetHidden(bool value);
  bool IsHidden();

  bool IsDirty() { return dirty; }
  void SetDirty(bool value) { dirty = value; }

  void SetState(ChunkState value);
  ChunkState GetState();

  glm::ivec2 &GetChunkPos();

  int GetSurfaceHeight(int x, int z);
private:
  [[maybe_unused]]World &world;

  Block blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_LENGTH];
  std::vector<FaceGeometry> translucentFaces;

  GLuint opaqueVAO, opaqueVBO, opaqueEBO;
  GLuint transVAO, transVBO, transEBO;

  glm::ivec2 chunkPos;

  bool hidden = true;
  ChunkState state = ChunkState::Unloaded;

  bool dirty = true;

  bool hasTranslucentBlocks = false;

  int opaqueVertexCount = 0, transVertexCount = 0;

  // void InitializeBuffers(GLuint &VAO, GLuint &VBO, GLuint &EBO);
  void InitializeOpaque();
  void InitializeTranslucent();
};

}

#endif // CHUNK_H_
