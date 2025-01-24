#include "World/Chunk.h"
#include "Utils/Profiler.h"
#include "Utils/defs.h"
#include "World/Block.h"
#include "World/BlockAtlas.h"
#include "World/World.h"
#include "World/BlockFace.h"

namespace World {

void Chunk::Initialize() {
  // Set all blocks to be empty
  for (int x = 0; x < CHUNK_WIDTH; ++x) {
    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
      for (int z = 0; z < CHUNK_LENGTH; ++z) {
        SetBlockAt(glm::vec3(x, y, z), Block(BlockType::AIR));
      }
    }
  }

  // Set up buffers
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

  // positions
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *) 0);
  glEnableVertexAttribArray(0);

  // texture coords
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *) (3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  // normals
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *) (5 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}

void Chunk::Render(Graphics::Texture &blockAtlasTexture, GLuint depthMap, Graphics::Shader &blockShader, const glm::vec3 &playerPos) {
  blockAtlasTexture.Bind(0);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, depthMap);

  blockShader.Use();

  blockShader.Uniform("uBlockAtlas", 0);
  blockShader.Uniform("uCameraPos", playerPos);

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(chunkPos.x * CHUNK_WIDTH, 0.0f, chunkPos.y * CHUNK_LENGTH));
  blockShader.Uniform("uModel", model);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Chunk::UpdateMesh() {
  PROFILE_FUNCTION(Chunk)

  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;
  GLuint indexOffset = 0;

  for (int z = 0; z < CHUNK_LENGTH; ++z) {
    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
      for (int x = 0; x < CHUNK_WIDTH; ++x) {

        const Block block = GetBlockAt(glm::vec3(x, y, z));

        if (!block.IsSolid()) continue;

        for (const BlockFace &face : BlockFace::allFaces) {

          if (!world.IsFaceVisible(face, glm::vec3(chunkPos.x * CHUNK_WIDTH + x, y, chunkPos.y * CHUNK_LENGTH + z))) {
            continue;
          }

          const std::array<glm::vec3, 4> faceVertices = face.GetVertices();
          const glm::vec2 topLeftTexCoord = BlockAtlas::GetNormalizedTextureCoords(block.GetType(), face);

          const glm::vec2 texSize = BlockAtlas::tileSize;
          const glm::vec3 normal = face.GetNormal();

          const std::array<glm::vec2, 4> faceTexCoords = {
            topLeftTexCoord,                                                  // Top-left
            { topLeftTexCoord.x, topLeftTexCoord.y + texSize.y },             // Bottom-left
            { topLeftTexCoord.x + texSize.x, topLeftTexCoord.y + texSize.y }, // Bottom-right
            { topLeftTexCoord.x + texSize.x, topLeftTexCoord.y }              // Top-right
          };

          for (int i = 0; i < 4; ++i) {
            vertices.insert(vertices.end(),{
              faceVertices[i].x + x, faceVertices[i].y + y, faceVertices[i].z + z,  // positions
              faceTexCoords[i].x, faceTexCoords[i].y,  // tex coords
              normal.x, normal.y, normal.z  // normal
            });
          }

          indices.insert(indices.end(), {
            indexOffset + 0, indexOffset + 1, indexOffset + 2,
            indexOffset + 2, indexOffset + 3, indexOffset + 0,
          });
          indexOffset += 4;
        }
      }
    }
  }

  glBindVertexArray(VAO);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(GLfloat)), vertices.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)), indices.data(), GL_DYNAMIC_DRAW);

  vertexCount = indices.size();
  
  vertices.clear();
  indices.clear();
}


Block &Chunk::GetBlockAt(GLuint x, GLuint y, GLuint z) {
  return blocks[x][y][z];
}

Block &Chunk::GetBlockAt(const glm::vec3 &pos) {
  return GetBlockAt(pos.x, pos.y, pos.z);
}

void Chunk::SetBlockAt(GLuint x, GLuint y, GLuint z, const Block &block) {
  blocks[x][y][z] = block;
}

void Chunk::SetBlockAt(const glm::vec3 &pos, const Block &block) {
  return SetBlockAt(pos.x, pos.y, pos.z, block);
}

GLuint Chunk::GetMeshVAO() const {
  return VAO;
}

GLsizei Chunk::GetVertexCount() {
  return vertexCount;
}

void Chunk::SetHidden(bool value) {
  hidden = value;
}

bool Chunk::IsHidden() {
  return hidden;
}

void Chunk::SetState(ChunkState value) {
  state = value;
}

ChunkState Chunk::GetState() {
  return state;
}

glm::ivec2 &Chunk::GetChunkPos() {
  return chunkPos;
}

}
