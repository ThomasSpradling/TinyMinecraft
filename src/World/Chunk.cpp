#include "World/Chunk.h"
#include "Graphics/gfx.h"
#include "Utils/Profiler.h"
#include "Utils/defs.h"
#include "Utils/utils.h"
#include "World/Block.h"
#include "World/BlockAtlas.h"
#include "World/World.h"
#include "World/BlockFace.h"
#include "glm/geometric.hpp"
#include <algorithm>
#include <iostream>
#include <limits>

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

  InitializeOpaque();
  InitializeTranslucent();
}

void Chunk::InitializeOpaque() {
  glGenVertexArrays(1, &opaqueVAO);
  glGenBuffers(1, &opaqueVBO);
  glGenBuffers(1, &opaqueEBO);

  glBindVertexArray(opaqueVAO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opaqueEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, opaqueVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

  // positions
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (void *) 0);
  glEnableVertexAttribArray(0);

  // colors
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (void *) (3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  // texture coords
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (void *) (7 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);

  // normals
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (void *) (9 * sizeof(GLfloat)));
  glEnableVertexAttribArray(3);

  glBindVertexArray(0);
}

void Chunk::InitializeTranslucent() {
  glGenVertexArrays(1, &transVAO);
  glGenBuffers(1, &transVBO);
  glGenBuffers(1, &transEBO);

  glBindVertexArray(transVAO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, transEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, transVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

  // positions
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void *) 0);
  glEnableVertexAttribArray(0);

  // colors
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void *) (3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  // texture coords
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void *) (7 * sizeof(GLfloat)));
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

  glBindVertexArray(opaqueVAO);
  glDrawElements(GL_TRIANGLES, opaqueVertexCount, GL_UNSIGNED_INT, 0);
}

void Chunk::RenderTranslucent(Graphics::Texture &blockAtlasTexture, GLuint depthMap, Graphics::Shader &shader, const glm::vec3 &playerPos) {
  glDisable(GL_CULL_FACE);
  
  shader.Use();
  shader.Uniform("uBlockAtlas", 0);

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(chunkPos.x * CHUNK_WIDTH, 0.0f, chunkPos.y * CHUNK_LENGTH));
  shader.Uniform("uModel", model);
  glBindVertexArray(transVAO);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthMask(GL_FALSE);

  glDrawElements(GL_TRIANGLES, transVertexCount, GL_UNSIGNED_INT, 0);

  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);

  glBindVertexArray(0);

  glEnable(GL_CULL_FACE);
}

void Chunk::UpdateMesh() {
  PROFILE_FUNCTION(Chunk)

  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;
  GLuint indexOffset = 0;

  hasTranslucentBlocks = false;

  for (int z = 0; z < CHUNK_LENGTH; ++z) {
    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
      for (int x = 0; x < CHUNK_WIDTH; ++x) {
        const Block block = GetBlockAt(glm::vec3(x, y, z));

        if (block.GetType() == BlockType::AIR) {
          continue;
        }

        if (block.IsTransparent()) {
          hasTranslucentBlocks = true;
          continue;
        }

        for (const BlockFace &face : BlockFace::allFaces) {

          if (!world.IsFaceVisible(block, face, glm::vec3(chunkPos.x * CHUNK_WIDTH + x, y, chunkPos.y * CHUNK_LENGTH + z))) {
            continue;
          }

          const std::array<glm::vec3, 4> faceVertices = face.GetVertices();
          const glm::vec2 topLeftTexCoord = BlockAtlas::GetNormalizedTextureCoords(block.GetType(), face);

          const glm::vec2 texSize = BlockAtlas::tileSize;
          const glm::vec3 normal = face.GetNormal();

          const glm::vec4 blockColor { 0.0f };

          const std::array<glm::vec2, 4> faceTexCoords = {
            topLeftTexCoord,                                                  // Top-left
            { topLeftTexCoord.x, topLeftTexCoord.y + texSize.y },             // Bottom-left
            { topLeftTexCoord.x + texSize.x, topLeftTexCoord.y + texSize.y }, // Bottom-right
            { topLeftTexCoord.x + texSize.x, topLeftTexCoord.y }              // Top-right
          };

          for (int i = 0; i < 4; ++i) {
            vertices.insert(vertices.end(),{
              faceVertices[i].x + x, faceVertices[i].y + y, faceVertices[i].z + z,  // positions
              blockColor.r, blockColor.g, blockColor.b, blockColor.a,  // colors
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

  glBindVertexArray(opaqueVAO);
  
  glBindBuffer(GL_ARRAY_BUFFER, opaqueVBO);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(GLfloat)), vertices.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opaqueEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)), indices.data(), GL_DYNAMIC_DRAW);

  opaqueVertexCount = indices.size();
  
  vertices.clear();
  indices.clear();
}

void Chunk::UpdateTranslucentMesh(const glm::vec3 &playerPos) {
  PROFILE_FUNCTION(Chunk)

  translucentFaces.clear();

  for (int z = 0; z < CHUNK_LENGTH; ++z) {
    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
      for (int x = 0; x < CHUNK_WIDTH; ++x) {

        glm::vec3 pos { x, y, z };

        const Block block = GetBlockAt(glm::vec3(x, y, z));

        if (block.GetType() == BlockType::AIR) continue;

        if (!block.IsTransparent()) {
          continue;
        }

        glm::vec3 blockPos = glm::vec3(CHUNK_WIDTH * chunkPos.x, 0, CHUNK_LENGTH * chunkPos.y) + glm::vec3(pos);

        if (block.IsBillboard()) {
          FaceGeometry faceGeom1;
          FaceGeometry faceGeom2;

          const glm::vec4 blockColor { 0.0f };

          const glm::vec2 topLeftTexCoord = BlockAtlas::GetNormalizedTextureCoords(block.GetType(), BlockFace::Direction::None);
          const glm::vec2 texSize = BlockAtlas::tileSize;

          const std::array<glm::vec2, 4> faceTexCoords = {
            topLeftTexCoord,                                                  // Top-left
            { topLeftTexCoord.x, topLeftTexCoord.y + texSize.y },             // Bottom-left
            { topLeftTexCoord.x + texSize.x, topLeftTexCoord.y + texSize.y }, // Bottom-right
            { topLeftTexCoord.x + texSize.x, topLeftTexCoord.y }              // Top-right
          };

          std::array<glm::vec3, 4> faceVertices1 = {
            glm::vec3(0, 1, 1),
            glm::vec3(0, 0, 1),
            glm::vec3(1, 0, 0),
            glm::vec3(1, 1, 0),
          };
          std::array<glm::vec3, 4> faceVertices2 = {
            glm::vec3(0, 1, 0),
            glm::vec3(0, 0, 0),
            glm::vec3(1, 0, 1),
            glm::vec3(1, 1, 1),
          };

          faceGeom1.pos = blockPos + Utils::CalculateConvexCenter<4>(faceVertices1);
          faceGeom2.pos = blockPos + Utils::CalculateConvexCenter<4>(faceVertices2);

          for (int i = 0; i < 4; ++i) {
            faceGeom1.vertices.insert(faceGeom1.vertices.end(), {
              faceVertices1[i].x + x, faceVertices1[i].y + y, faceVertices1[i].z + z,  // positions
              blockColor.r, blockColor.g, blockColor.b, blockColor.a,  // colors
              faceTexCoords[i].x, faceTexCoords[i].y,  // tex coords
            });
            faceGeom2.vertices.insert(faceGeom2.vertices.end(), {
              faceVertices2[i].x + x, faceVertices2[i].y + y, faceVertices2[i].z + z,  // positions
              blockColor.r, blockColor.g, blockColor.b, blockColor.a,  // colors
              faceTexCoords[i].x, faceTexCoords[i].y,  // tex coords
            });
          }

          faceGeom1.indices.insert(faceGeom1.indices.end(), {
            0, 1, 2,
            2, 3, 0,
          });

          faceGeom2.indices.insert(faceGeom2.indices.end(), {
            0, 1, 2,
            2, 3, 0,
          });

          faceGeom1.distanceToPlayer = glm::distance(playerPos, faceGeom1.pos);
          translucentFaces.push_back(faceGeom1);

          faceGeom2.distanceToPlayer = glm::distance(playerPos, faceGeom2.pos);
          translucentFaces.push_back(faceGeom2);

          continue;
        }

        for (const BlockFace &face : BlockFace::allFaces) {

          if (!world.IsFaceVisible(block, face, glm::vec3(chunkPos.x * CHUNK_WIDTH + x, y, chunkPos.y * CHUNK_LENGTH + z))) {
            continue;
          }

          FaceGeometry faceGeom;

          std::array<glm::vec3, 4> faceVertices = block.GetType() == WATER ?
            face.GetFluidVertices() : face.GetVertices();

          faceGeom.pos = blockPos + Utils::CalculateConvexCenter<4>(faceVertices);
          
          const glm::vec2 topLeftTexCoord = BlockAtlas::GetNormalizedTextureCoords(block.GetType(), face);

          const glm::vec2 texSize = BlockAtlas::tileSize;
          const glm::vec3 normal = face.GetNormal();

          const glm::vec4 blockColor { 0.0f };

          const std::array<glm::vec2, 4> faceTexCoords = {
            topLeftTexCoord,                                                  // Top-left
            { topLeftTexCoord.x, topLeftTexCoord.y + texSize.y },             // Bottom-left
            { topLeftTexCoord.x + texSize.x, topLeftTexCoord.y + texSize.y }, // Bottom-right
            { topLeftTexCoord.x + texSize.x, topLeftTexCoord.y }              // Top-right
          };

          for (int i = 0; i < 4; ++i) {
            faceGeom.vertices.insert(faceGeom.vertices.end(), {
              faceVertices[i].x + x, faceVertices[i].y + y, faceVertices[i].z + z,  // positions
              blockColor.r, blockColor.g, blockColor.b, blockColor.a,  // colors
              faceTexCoords[i].x, faceTexCoords[i].y,  // tex coords
            });
          }

          faceGeom.indices.insert(faceGeom.indices.end(), {
            0, 1, 2,
            2, 3, 0,
          });

          faceGeom.distanceToPlayer = glm::distance(playerPos, faceGeom.pos);

          translucentFaces.push_back(faceGeom);
        }

      }
    }
  }

  SortTranslucentBlocks(playerPos);
}

void Chunk::SortTranslucentBlocks(const glm::vec3 &playerPos) {
  if (!HasTranslucentBlocks()) {
    return;
  }

  std::sort(translucentFaces.begin(), translucentFaces.end(), [playerPos](const FaceGeometry &a, const FaceGeometry &b) {
    float distanceA = glm::distance(playerPos, a.pos);
    float distanceB = glm::distance(playerPos, b.pos);

    return distanceA > distanceB;
  });

  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;

  GLuint indexOffset = 0;

  for (const auto &faceGeom : translucentFaces) {
    vertices.insert(vertices.end(), faceGeom.vertices.begin(), faceGeom.vertices.end());

    for (GLuint index : faceGeom.indices) {
      indices.push_back(index + indexOffset);
    }

    indexOffset += static_cast<GLuint>(faceGeom.vertices.size() / 8);
  }

  glBindVertexArray(transVAO);

  glBindBuffer(GL_ARRAY_BUFFER, transVBO);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(GLfloat)), vertices.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, transEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)), indices.data(), GL_DYNAMIC_DRAW);

  transVertexCount = indices.size();
  
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

bool Chunk::HasTranslucentBlocks() {
  return hasTranslucentBlocks;
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

int Chunk::GetSurfaceHeight(int x, int z) {
  for (int y = CHUNK_HEIGHT - 1; y >= 0; --y) {
    if (GetBlockAt(x, y, z).GetType() != BlockType::AIR) {
      return y;
    }
  }

  return 0;
}

}
