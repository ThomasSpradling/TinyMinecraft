#include "World/Chunk.h"
#include "Geometry/Mesh.h"
#include "Geometry/geometry.h"
#include "Graphics/gfx.h"
#include "Utils/Profiler.h"
#include "Utils/defs.h"
#include "Utils/utils.h"
#include "World/Block.h"
#include "World/BlockAtlas.h"
#include "World/World.h"
#include "glm/geometric.hpp"
#include <algorithm>
#include <memory>

namespace World {

Chunk::Chunk(World &world, const glm::ivec2 &m_chunkPos)
  : m_world(world)
  , m_chunkPos(m_chunkPos)
{
  m_opaqueMesh = std::make_unique<Geometry::Mesh>();
  m_translucentMesh = std::make_unique<Geometry::Mesh>();

  for (int x = 0; x < CHUNK_WIDTH; ++x) {
    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
      for (int z = 0; z < CHUNK_LENGTH; ++z) {
        SetBlockAt(glm::vec3(x, y, z), Block(BlockType::AIR));
      }
    }
  }
}

Chunk::Chunk(Chunk &&other) noexcept
  : m_world(other.m_world)
  , m_data(std::move(other.m_data))
  , m_opaqueMesh(std::move(other.m_opaqueMesh))
  , m_translucentMesh(std::move(other.m_translucentMesh))
  , m_chunkPos(other.m_chunkPos)
{}

// Move assignment operator
auto Chunk::operator=(Chunk &&other) noexcept -> Chunk & {
  Chunk temp(std::move(other));
  std::swap(*this, temp);
  return *this;
}

void Chunk::UpdateMesh() {
  PROFILE_FUNCTION(Chunk)

  constexpr std::array<Geometry::Face, 6> faces = {
    Geometry::Face::Up, Geometry::Face::Down, Geometry::Face::East,
    Geometry::Face::West, Geometry::Face::North, Geometry::Face::South
  };

  // std::vector<Geometry::MeshVertex> vertices;
  // std::vector<GLuint> indices;
  GLuint indexOffset = 0;

  m_hasTranslucentBlocks = false;

  for (int z = 0; z < CHUNK_LENGTH; ++z) {
    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
      for (int x = 0; x < CHUNK_WIDTH; ++x) {
        const glm::vec3 pos = glm::vec3(x, y, z);
        const Block block = GetBlockAt(pos);

        if (block.GetType() == BlockType::AIR) continue;

        if (block.IsTransparent()) {
          m_hasTranslucentBlocks = true;
          continue;
        }

        for (Geometry::Face face : faces) {

          if (!m_world.IsFaceVisible(block, face, GetGlobalCoords(pos))) continue;

          const std::array<glm::vec3, 4> faceVertices = Geometry::GetVertices(face);
          const glm::vec2 topLeftTexCoord = BlockAtlas::GetNormalizedTextureCoords(block.GetType(), face);
          const glm::vec3 normal = GetNormal(face);
          const glm::vec4 blockColor { 0.0f };

          const std::array<glm::vec2, 4> faceTexCoords = {
            topLeftTexCoord,                                                                              // Top-left
            { topLeftTexCoord.x, topLeftTexCoord.y + BlockAtlas::tileSize.y },                            // Bottom-left
            { topLeftTexCoord.x + BlockAtlas::tileSize.x, topLeftTexCoord.y + BlockAtlas::tileSize.y },   // Bottom-right
            { topLeftTexCoord.x + BlockAtlas::tileSize.x, topLeftTexCoord.y }                             // Top-right
          };

          // push verts and indices

          for (int i = 0; i < 4; ++i) {
            m_opaqueVertices.insert(m_opaqueVertices.end(), {
              glm::vec3(x, y, z) + faceVertices.at(i),
              blockColor,
              faceTexCoords[i],
              normal
            });
          }

          m_opaqueIndices.insert(m_opaqueIndices.end(), {
            indexOffset + 0, indexOffset + 1, indexOffset + 2,
            indexOffset + 2, indexOffset + 3, indexOffset + 0,
          });
          indexOffset += 4;
        }
      }
    }
  }

  // cleanup

  // vertices.clear();
  // indices.clear();
}

void Chunk::BufferVertices() {
  m_opaqueMesh->Update(m_opaqueVertices, m_opaqueIndices);
}

void Chunk::UpdateTranslucentMesh(const glm::vec3 &playerPos) {
  PROFILE_FUNCTION(Chunk)

  m_data.translucentFaces.clear();
  constexpr std::array<Geometry::Face, 6> faces = {
    Geometry::Face::Up, Geometry::Face::Down, Geometry::Face::East,
    Geometry::Face::West, Geometry::Face::North, Geometry::Face::South
  };

  for (int z = 0; z < CHUNK_LENGTH; ++z) {
    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
      for (int x = 0; x < CHUNK_WIDTH; ++x) {

        const glm::vec3 pos = glm::vec3(x, y, z);
        const Block block = GetBlockAt(pos);

        if (block.GetType() == BlockType::AIR) continue;
        if (!block.IsTransparent()) continue;

        const glm::vec3 blockPos = GetGlobalCoords(pos);

        if (block.IsBillboard()) {
          FaceGeometry faceGeom1;
          FaceGeometry faceGeom2;

          const glm::vec4 blockColor { 0.0f };
          const glm::vec2 topLeftTexCoord = BlockAtlas::GetNormalizedTextureCoords(block.GetType(), Geometry::Face::None);
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
              glm::vec3(x, y, z) + faceVertices1.at(i),
              blockColor,
              faceTexCoords[i],
              glm::vec3(0.0f)
            });
            faceGeom2.vertices.insert(faceGeom2.vertices.end(), {
              glm::vec3(x, y, z) + faceVertices2.at(i),
              blockColor,
              faceTexCoords[i],
              glm::vec3(0.0f)
            });
          }

          GLuint indexOffset = 0;

          faceGeom1.indices.insert(faceGeom1.indices.end(), {
            0, 1, 2,
            2, 3, 0,
          });

          faceGeom2.indices.insert(faceGeom2.indices.end(), {
            0, 1, 2,
            2, 3, 0,
          });

          faceGeom1.distanceToPlayer = glm::distance(playerPos, faceGeom1.pos);
          m_data.translucentFaces.push_back(faceGeom1);

          faceGeom2.distanceToPlayer = glm::distance(playerPos, faceGeom2.pos);
          m_data.translucentFaces.push_back(faceGeom2);

          continue;
        }

        for (Geometry::Face face : faces) {

          if (!m_world.IsFaceVisible(block, face, glm::vec3(m_chunkPos.x * CHUNK_WIDTH + x, y, m_chunkPos.y * CHUNK_LENGTH + z))) {
            continue;
          }

          FaceGeometry faceGeom;
          
          std::array<glm::vec3, 4> faceVertices = block.IsFluid() ? Geometry::GetFluidVertices(face) : Geometry::GetVertices(face);

          faceGeom.pos = blockPos + Utils::CalculateConvexCenter<4>(faceVertices);
          
          const glm::vec2 topLeftTexCoord = BlockAtlas::GetNormalizedTextureCoords(block.GetType(), face);

          const glm::vec2 texSize = BlockAtlas::tileSize;
          const glm::vec3 normal = GetNormal(face);

          const glm::vec4 blockColor { 0.0f };

          const std::array<glm::vec2, 4> faceTexCoords = {
            topLeftTexCoord,                                                  // Top-left
            { topLeftTexCoord.x, topLeftTexCoord.y + texSize.y },             // Bottom-left
            { topLeftTexCoord.x + texSize.x, topLeftTexCoord.y + texSize.y }, // Bottom-right
            { topLeftTexCoord.x + texSize.x, topLeftTexCoord.y }              // Top-right
          };

          for (int i = 0; i < 4; ++i) {
            faceGeom.vertices.insert(faceGeom.vertices.end(), {
              glm::vec3(x, y, z) + faceVertices[i],
              blockColor,
              faceTexCoords[i],
              normal
            });
          }

          faceGeom.indices.insert(faceGeom.indices.end(), {
            0, 1, 2,
            2, 3, 0,
          });

          faceGeom.distanceToPlayer = glm::distance(playerPos, faceGeom.pos);

          m_data.translucentFaces.push_back(faceGeom);
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

  std::ranges::sort(m_data.translucentFaces, [playerPos](const FaceGeometry &a, const FaceGeometry &b) {
    float distanceA = glm::distance(playerPos, a.pos);
    float distanceB = glm::distance(playerPos, b.pos);

    return distanceA > distanceB;
  });

  std::vector<Geometry::MeshVertex> vertices;
  std::vector<GLuint> indices;

  GLuint indexOffset = 0;

  for (const auto &faceGeom : m_data.translucentFaces) {
    vertices.insert(vertices.end(), faceGeom.vertices.begin(), faceGeom.vertices.end());

    for (GLuint index : faceGeom.indices) {
      indices.push_back(index + indexOffset);
    }

    indexOffset += 4;
  }

  m_translucentMesh->Update(vertices, indices);
  
  vertices.clear();
  indices.clear();
}

auto Chunk::GetBlockAt(int x, int y, int z) -> Block & {
  return m_data.blocks.at(CHUNK_LENGTH * CHUNK_HEIGHT * x + CHUNK_LENGTH * y + z);
}

auto Chunk::GetBlockAt(const glm::ivec3 &pos) -> Block & {
  return GetBlockAt(pos.x, pos.y, pos.z);
}

void Chunk::SetBlockAt(int x, int y, int z, const Block &block) {
  m_data.blocks.at(CHUNK_LENGTH * CHUNK_HEIGHT * x + CHUNK_LENGTH * y + z) = block;
}

void Chunk::SetBlockAt(const glm::ivec3 &pos, const Block &block) {
  return SetBlockAt(pos.x, pos.y, pos.z, block);
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
