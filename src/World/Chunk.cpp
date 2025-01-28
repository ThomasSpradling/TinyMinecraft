#include "World/Chunk.h"
#include "Graphics/gfx.h"
#include "Utils/Logger.h"
#include "Utils/Profiler.h"
#include "Utils/defs.h"
#include "Utils/utils.h"
#include "World/Block.h"
#include "World/BlockAtlas.h"
#include "World/World.h"
#include "World/BlockFace.h"
#include "glm/geometric.hpp"
#include <algorithm>

namespace World {

Chunk::Chunk(World &world, const glm::ivec2 &m_chunkPos)
  :m_world(world)
  , m_opaqueVBO(GL_ARRAY_BUFFER)
  , m_transVBO(GL_ARRAY_BUFFER)
  , m_opaqueEBO(GL_ELEMENT_ARRAY_BUFFER)
  , m_transEBO(GL_ELEMENT_ARRAY_BUFFER)
  , m_chunkPos(m_chunkPos)
{
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

Chunk::Chunk(Chunk &&other) noexcept
  : m_world(other.m_world)
  , m_translucentFaces(std::move(other.m_translucentFaces))
  , m_opaqueVAO(std::move(other.m_opaqueVAO))
  , m_transVAO(std::move(other.m_transVAO))
  , m_opaqueVBO(std::move(other.m_opaqueVBO))
  , m_transVBO(std::move(other.m_transVBO))
  , m_opaqueEBO(std::move(other.m_opaqueEBO))
  , m_transEBO(std::move(other.m_transEBO))
  , m_chunkPos(other.m_chunkPos)
{
}

// Move assignment operator
auto Chunk::operator=(Chunk &&other) noexcept -> Chunk & {
  Chunk temp(std::move(other));
  std::swap(*this, temp);
  return *this;
}

void Chunk::InitializeOpaque() {
  m_opaqueVAO.Bind();
  m_opaqueVBO.Bind();
  m_opaqueEBO.Bind();

  m_opaqueVAO.AddAttribute(0, 3, GL_FLOAT, 12 * sizeof(GLfloat), 0);
  m_opaqueVAO.AddAttribute(1, 4, GL_FLOAT, 12 * sizeof(GLfloat), 3 * sizeof(GLfloat));
  m_opaqueVAO.AddAttribute(2, 2, GL_FLOAT, 12 * sizeof(GLfloat), 7 * sizeof(GLfloat));
  m_opaqueVAO.AddAttribute(3, 3, GL_FLOAT, 12 * sizeof(GLfloat), 9 * sizeof(GLfloat));
}

void Chunk::InitializeTranslucent() {
  m_transVAO.Bind();
  m_transVBO.Bind();
  m_transEBO.Bind();

  m_transVAO.AddAttribute(0, 3, GL_FLOAT, 9 * sizeof(GLfloat), 0);
  m_transVAO.AddAttribute(1, 4, GL_FLOAT, 9 * sizeof(GLfloat), 3 * sizeof(GLfloat));
  m_transVAO.AddAttribute(2, 2, GL_FLOAT, 9 * sizeof(GLfloat), 7 * sizeof(GLfloat));
}

void Chunk::Render(Graphics::Texture &blockAtlasTexture, Graphics::Shader &blockShader, const glm::vec3 &playerPos) {
  blockAtlasTexture.Bind();
  glActiveTexture(GL_TEXTURE2);
  // glBindTexture(GL_TEXTURE_2D, depthMap);

  blockShader.Use();

  blockShader.Uniform("uBlockAtlas", 0);
  blockShader.Uniform("uCameraPos", playerPos);

  glm::mat4 model { 1.0f };
  model = glm::translate(model, glm::vec3(m_chunkPos.x * CHUNK_WIDTH, 0.0f, m_chunkPos.y * CHUNK_LENGTH));
  blockShader.Uniform("uModel", model);

  m_opaqueVAO.Bind();
  glDrawElements(GL_TRIANGLES, static_cast<int>(m_opaqueVertexCount), GL_UNSIGNED_INT, nullptr);
}

void Chunk::RenderTranslucent(Graphics::Texture &blockAtlasTexture, Graphics::Shader &shader, const glm::vec3 &playerPos) {
  glDisable(GL_CULL_FACE);
  
  shader.Use();
  shader.Uniform("uBlockAtlas", static_cast<int>(blockAtlasTexture.GetId()));

  auto model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(m_chunkPos.x * CHUNK_WIDTH, 0.0f, m_chunkPos.y * CHUNK_LENGTH));
  shader.Uniform("uModel", model);

  m_transVAO.Bind();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthMask(GL_FALSE);

  glDrawElements(GL_TRIANGLES, static_cast<int>(m_transVertexCount), GL_UNSIGNED_INT, nullptr);

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

  m_hasTranslucentBlocks = false;

  for (int z = 0; z < CHUNK_LENGTH; ++z) {
    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
      for (int x = 0; x < CHUNK_WIDTH; ++x) {
        const Block block = GetBlockAt(glm::vec3(x, y, z));

        if (block.GetType() == BlockType::AIR) {
          continue;
        }

        if (block.IsTransparent()) {
          m_hasTranslucentBlocks = true;
          continue;
        }

        for (const BlockFace &face : BlockFace::allFaces) {

          if (!m_world.IsFaceVisible(block, face, glm::vec3(m_chunkPos.x * CHUNK_WIDTH + x, y, m_chunkPos.y * CHUNK_LENGTH + z))) {
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
              faceVertices.at(i).x + static_cast<float>(x), faceVertices.at(i).y + static_cast<float>(y), faceVertices.at(i).z + static_cast<float>(z),  // positions
              blockColor.r, blockColor.g, blockColor.b, blockColor.a,  // colors
              faceTexCoords.at(i).x, faceTexCoords.at(i).y,  // tex coords
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

  m_opaqueVAO.Bind();
  
  m_opaqueVBO.BufferData(vertices, GL_DYNAMIC_DRAW);
  m_opaqueEBO.BufferData(indices, GL_DYNAMIC_DRAW);

  m_opaqueVertexCount = indices.size();
  
  vertices.clear();
  indices.clear();
}

void Chunk::UpdateTranslucentMesh(const glm::vec3 &playerPos) {
  PROFILE_FUNCTION(Chunk)

  m_translucentFaces.clear();

  for (int z = 0; z < CHUNK_LENGTH; ++z) {
    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
      for (int x = 0; x < CHUNK_WIDTH; ++x) {

        glm::vec3 pos { x, y, z };

        const Block block = GetBlockAt(glm::vec3(x, y, z));

        if (block.GetType() == BlockType::AIR) continue;

        if (!block.IsTransparent()) {
          continue;
        }

        glm::vec3 blockPos = glm::vec3(CHUNK_WIDTH * m_chunkPos.x, 0, CHUNK_LENGTH * m_chunkPos.y) + glm::vec3(pos);

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
              faceVertices1.at(i).x + static_cast<float>(x), faceVertices1.at(i).y + static_cast<float>(y), faceVertices1.at(i).z + static_cast<float>(z),  // positions
              blockColor.r, blockColor.g, blockColor.b, blockColor.a,  // colors
              faceTexCoords.at(i).x, faceTexCoords.at(i).y,  // tex coords
            });
            faceGeom2.vertices.insert(faceGeom2.vertices.end(), {
              faceVertices2.at(i).x + static_cast<float>(x), faceVertices2.at(i).y + static_cast<float>(y), faceVertices2.at(i).z + static_cast<float>(z),  // positions
              blockColor.r, blockColor.g, blockColor.b, blockColor.a,  // colors
              faceTexCoords.at(i).x, faceTexCoords.at(i).y,  // tex coords
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
          m_translucentFaces.push_back(faceGeom1);

          faceGeom2.distanceToPlayer = glm::distance(playerPos, faceGeom2.pos);
          m_translucentFaces.push_back(faceGeom2);

          continue;
        }

        for (const BlockFace &face : BlockFace::allFaces) {

          if (!m_world.IsFaceVisible(block, face, glm::vec3(m_chunkPos.x * CHUNK_WIDTH + x, y, m_chunkPos.y * CHUNK_LENGTH + z))) {
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
              faceVertices.at(i).x + static_cast<float>(x), faceVertices.at(i).y + static_cast<float>(y), faceVertices.at(i).z + static_cast<float>(z),  // positions
              blockColor.r, blockColor.g, blockColor.b, blockColor.a,  // colors
              faceTexCoords.at(i).x, faceTexCoords.at(i).y,  // tex coords
            });
          }

          faceGeom.indices.insert(faceGeom.indices.end(), {
            0, 1, 2,
            2, 3, 0,
          });

          faceGeom.distanceToPlayer = glm::distance(playerPos, faceGeom.pos);

          m_translucentFaces.push_back(faceGeom);
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

  std::ranges::sort(m_translucentFaces, [playerPos](const FaceGeometry &a, const FaceGeometry &b) {
    float distanceA = glm::distance(playerPos, a.pos);
    float distanceB = glm::distance(playerPos, b.pos);

    return distanceA > distanceB;
  });

  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;

  GLuint indexOffset = 0;

  for (const auto &faceGeom : m_translucentFaces) {
    vertices.insert(vertices.end(), faceGeom.vertices.begin(), faceGeom.vertices.end());

    for (GLuint index : faceGeom.indices) {
      indices.push_back(index + indexOffset);
    }

    indexOffset += static_cast<GLuint>(faceGeom.vertices.size() / 8);
  }

  m_transVAO.Bind();

  m_transVBO.BufferData(vertices, GL_DYNAMIC_DRAW);
  m_transEBO.BufferData(indices, GL_DYNAMIC_DRAW);

  m_transVertexCount = indices.size();
  
  vertices.clear();
  indices.clear();
}

auto Chunk::GetBlockAt(int x, int y, int z) -> Block & {
  return m_blocks.at(CHUNK_LENGTH * CHUNK_HEIGHT * x + CHUNK_LENGTH * y + z);
}

auto Chunk::GetBlockAt(const glm::ivec3 &pos) -> Block & {
  return GetBlockAt(pos.x, pos.y, pos.z);
}

void Chunk::SetBlockAt(int x, int y, int z, const Block &block) {
  m_blocks.at(CHUNK_LENGTH * CHUNK_HEIGHT * x + CHUNK_LENGTH * y + z) = block;
}

void Chunk::SetBlockAt(const glm::ivec3 &pos, const Block &block) {
  return SetBlockAt(pos.x, pos.y, pos.z, block);
}

auto Chunk::HasTranslucentBlocks() const -> bool {
  return m_hasTranslucentBlocks;
}

void Chunk::SetHidden(bool value) {
  m_hidden = value;
}

auto Chunk::IsHidden() const -> bool {
  return m_hidden;
}

auto Chunk::IsDirty() const -> bool {
  return m_dirty;
}

void Chunk::SetDirty(bool value) {
  m_dirty = value;
}

void Chunk::SetState(ChunkState value) {
  m_state = value;
}

auto Chunk::GetState() const -> ChunkState {
  return m_state;
}

auto Chunk::GetChunkPos() const -> glm::ivec2 {
  return m_chunkPos;
}

auto Chunk::GetSurfaceHeight(int x, int z) -> int {
  for (int y = CHUNK_HEIGHT - 1; y >= 0; --y) {
    if (GetBlockAt(x, y, z).GetType() != BlockType::AIR) {
      return y;
    }
  }

  return 0;
}

}
