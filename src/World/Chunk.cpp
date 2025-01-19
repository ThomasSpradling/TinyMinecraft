#include "World/Chunk.h"
#include "World/Block.h"
#include "World/BlockAtlas.h"
#include "World/BlockFace.h"
#include "World/World.h"
#include "glm/fwd.hpp"
#include <iostream>

namespace World {

void Chunk::Init() {
  // Set all blocks to be empty
  for (GLuint x = 0; x < CHUNK_WIDTH; ++x) {
    for (GLuint y = 0; y < CHUNK_HEIGHT; ++y) {
      for (GLuint z = 0; z < CHUNK_LENGTH; ++z) {
        SetBlockAt(x, y, z, Block(BlockType::AIR));
      }
    }
  }

  // Set up buffers
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);

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


void Chunk::UpdateMesh() {
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;
  GLuint indexOffset = 0;

  for (int x = 0; x < CHUNK_WIDTH; ++x) {
    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
      for (int z = 0; z < CHUNK_LENGTH; ++z) {
        Block block = GetBlockAt(x, y, z);
        if (block.IsSolid()) {
          for (const BlockFace face : BlockFace::allFaces) {
            if (world.IsFaceVisible(face, glm::vec3(chunkPos.x * CHUNK_WIDTH + x, y, chunkPos.y * CHUNK_LENGTH + z))) {
              std::array<glm::vec3, 4> faceVertices = face.GetUnitVertices();
              glm::vec2 topLeftTexCoord = BlockAtlas::GetNormalizedTextureCoords(block.GetType(), face);

              glm::vec2 texSize = BlockAtlas::GetTextureSize();
              glm::vec3 normal = face.GetNormal();

              std::array<glm::vec2, 4> faceTexCoords = {
                topLeftTexCoord,                                                  // Top-left
                { topLeftTexCoord.x, topLeftTexCoord.y + texSize.y },             // Bottom-left
                { topLeftTexCoord.x + texSize.x, topLeftTexCoord.y + texSize.y }, // Bottom-right
                { topLeftTexCoord.x + texSize.x, topLeftTexCoord.y }              // Top-right
              };

              for (GLuint i = 0; i < 4; ++i) {
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
    }
  }

  glBindVertexArray(VAO);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(GLfloat)), vertices.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)), indices.data(), GL_DYNAMIC_DRAW);

  vertexCount = indices.size();
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

void Chunk::SetDirty(bool value) {
  dirty = value;
}

bool Chunk::IsDirty() {
  return dirty;
}

void Chunk::SetLoaded(bool value) {
  loaded = value;
}

bool Chunk::IsLoaded() {
  return loaded;
}

glm::ivec2 &Chunk::GetChunkPos() {
  return chunkPos;
}

}
