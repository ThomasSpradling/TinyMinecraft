#include "World/Chunk.h"
#include "Geometry/Mesh.h"
#include "Geometry/geometry.h"
#include "Graphics/gfx.h"
#include "Utils/Logger.h"
#include "Utils/Profiler.h"
#include "Utils/defs.h"
#include "Utils/utils.h"
#include "World/Block.h"
#include "World/BlockAtlas.h"
#include "World/BlockType.h"
#include "World/World.h"
#include "glm/geometric.hpp"
#include <algorithm>
#include <memory>

namespace TinyMinecraft {

  namespace World {

    Chunk::Chunk(World &world, const glm::ivec2 &m_chunkPos)
      : m_world(world)
      , m_chunkPos(m_chunkPos)
    {
      m_opaqueMesh = std::make_unique<Geometry::Mesh>();
      m_translucentMesh = std::make_unique<Geometry::Mesh>();
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

      GLuint indexOffset = 0;

      m_hasTranslucentBlocks = false;

      for (int z = 0; z < CHUNK_LENGTH; ++z) {
        for (int y = 0; y < CHUNK_HEIGHT; ++y) {
          for (int x = 0; x < CHUNK_WIDTH; ++x) {
            const glm::vec3 pos = glm::vec3(x, y, z);
            const BlockType block = GetBlockAt(pos);
            
            if (BlockData::IsEmpty(block)) continue;

            if (BlockData::IsTranslucent(block)) {
              m_hasTranslucentBlocks = true;
              continue;
            }

            BlockRenderType renderType = BlockData::Get(block).renderType;
            if (renderType == BlockRenderType::Standard  || renderType == BlockRenderType::Fluid) {
              AppendOpaqueBlockGeometry(block, pos, indexOffset, m_opaqueVertices, m_opaqueIndices);
            } else if (renderType == BlockRenderType::Fluid) {
              AppendOpaqueFluidGeometry(block, pos, indexOffset, m_opaqueVertices, m_opaqueIndices);
            } else if (renderType == BlockRenderType::Foliage) {
              AppendFoliageGeometry(block, pos, indexOffset, m_opaqueVertices, m_opaqueIndices);
            }
          }
        }
      }
    }

    void Chunk::BufferVertices() {
      m_opaqueMesh->Update(m_opaqueVertices, m_opaqueIndices);
    }
 
    void Chunk::BufferTranslucentVertices(){
      m_translucentMesh->Update(m_translucentVertices, m_translucentIndices);
    }

    void Chunk::UpdateTranslucentMesh(const glm::vec3 &playerPos) {
      PROFILE_FUNCTION(Chunk)

      m_data.translucentFaces.clear();

      for (int z = 0; z < CHUNK_LENGTH; ++z) {
        for (int y = 0; y < CHUNK_HEIGHT; ++y) {
          for (int x = 0; x < CHUNK_WIDTH; ++x) {

            const glm::vec3 pos = glm::vec3(x, y, z);
            const BlockType block = GetBlockAt(pos);

            if (BlockData::IsEmpty(block)) continue;
            if (!BlockData::IsTranslucent(block)) continue;

            const BlockRenderType renderType = BlockData::Get(block).renderType;
            if (renderType == BlockRenderType::Standard) {
              AppendTranslucentBlockGeometry(block, pos, m_data.translucentFaces);
            } else if (renderType == BlockRenderType::Fluid) {
              AppendTranslucentFluidGeometry(block, pos, m_data.translucentFaces);
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

      GLuint indexOffset = 0;

      for (const auto &faceGeom : m_data.translucentFaces) {
        m_translucentVertices.insert(m_translucentVertices.end(), faceGeom.vertices.begin(), faceGeom.vertices.end());

        for (GLuint index : faceGeom.indices) {
          m_translucentIndices.push_back(index + indexOffset);
        }

        indexOffset += 4;
      }
    }

    void Chunk::ClearBuffers() {
      if (ShouldClear()) {
        m_opaqueMesh->ClearBuffers();
        m_translucentMesh->ClearBuffers();
        SetShouldClear(false);
      }
    }

    auto Chunk::GetSurfaceHeight(int x, int z) -> int {
      for (int y = CHUNK_HEIGHT - 1; y >= 0; --y) {
        if (GetBlockAt(x, y, z) != BlockType::Air) {
          return y;
        }
      }

      return 0;
    }

    auto Chunk::GetBlockUnbounded(const glm::ivec3 &pos) -> BlockType {
      constexpr auto WrapIndex = [](int x, int size) {
        int offset = x / size;
        int local  = x % size;
        if (local < 0) {
          local += size;
          --offset;
        }
        return std::pair<int,int>(offset, local);
      };

      if (pos.y < 0 || pos.y >= CHUNK_HEIGHT)
        return BlockType::Air;

      if (pos.x >= 0 && pos.x < CHUNK_WIDTH && pos.z >= 0 && pos.z < CHUNK_LENGTH)
        return GetBlockAt(pos);

      auto [offsetX, localX] = WrapIndex(pos.x, CHUNK_WIDTH);
      auto [offsetZ, localZ] = WrapIndex(pos.z, CHUNK_LENGTH);

      glm::ivec2 neighborChunkPos = m_chunkPos + glm::ivec2(offsetX, offsetZ);
      return m_world.GetChunkAt(neighborChunkPos)->GetBlockAt(localX, pos.y, localZ);
    }

    auto Chunk::IsFaceVisible(BlockType block, Geometry::Face face, const glm::vec3 &pos) -> bool {
      glm::ivec3 neighboringPos = pos + Geometry::GetNormal(face);
      BlockType neighboringBlock = GetBlockUnbounded(neighboringPos);

      if (neighboringBlock == BlockType::Air) {
        return true;
      }

      if (!BlockData::IsTranslucent(block)) {
        return BlockData::IsTranslucent(neighboringBlock);
      }

      if (BlockData::IsTranslucent(block)) {
        if (BlockData::IsTranslucent(neighboringBlock)) {
          return neighboringBlock != block;
        }
      }

      return false;
    }

    void Chunk::AppendOpaqueBlockGeometry(BlockType block, glm::vec3 pos, GLuint &indexOffset, std::vector<Geometry::MeshVertex> &vertices, std::vector<GLuint> &indices) {
      for (int i = Geometry::Face::First; i != Geometry::Face::Last; ++i) {
        auto face = static_cast<Geometry::Face>(i);
        if (!IsFaceVisible(block, face, pos)) continue;

        const std::array<glm::vec3, 4> faceVertices = Geometry::GetVertices(face);
        const glm::vec2 topLeftTexCoord = BlockAtlas::GetNormalizedTextureCoords(block, face);
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
          vertices.insert(m_opaqueVertices.end(), {
            pos + faceVertices.at(i),
            blockColor,
            faceTexCoords[i],
            normal
          });
        }

        indices.insert(m_opaqueIndices.end(), {
          indexOffset + 0, indexOffset + 1, indexOffset + 2,
          indexOffset + 2, indexOffset + 3, indexOffset + 0,
        });
        indexOffset += 4;
      }
    }
    void Chunk::AppendTranslucentBlockGeometry(BlockType block, glm::vec3 pos, std::vector<FaceGeometry> &translucentFaces) {
      const glm::vec3 blockPos = GetGlobalCoords(pos);

      for (int i = Geometry::Face::First; i != Geometry::Face::Last; ++i) {
        auto face = static_cast<Geometry::Face>(i);
        if (!IsFaceVisible(block, face, pos)) {
          continue;
        }

        FaceGeometry faceGeom;
        std::array<glm::vec3, 4> faceVertices = Geometry::GetVertices(face);

        faceGeom.pos = blockPos + Utils::CalculateConvexCenter<4>(faceVertices);
        const glm::vec2 topLeftTexCoord = BlockAtlas::GetNormalizedTextureCoords(block, face);

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
            pos + faceVertices[i],
            blockColor,
            faceTexCoords[i],
            normal
          });
        }

        faceGeom.indices.insert(faceGeom.indices.end(), {
          0, 1, 2,
          2, 3, 0,
        });

        faceGeom.distanceToPlayer = glm::distance(m_world.GetPlayerPosition(), faceGeom.pos);

        translucentFaces.push_back(faceGeom);
      }
    }

    void Chunk::AppendOpaqueFluidGeometry(BlockType block, glm::vec3 pos, GLuint &indexOffset, std::vector<Geometry::MeshVertex> &vertices, std::vector<GLuint> &indices) {
      const glm::vec3 blockPos = GetGlobalCoords(pos);

      for (int i = Geometry::Face::First; i != Geometry::Face::Last; ++i) {
        auto face = static_cast<Geometry::Face>(i);
        if (!IsFaceVisible(block, face, pos)) {
          continue;
        }

        std::array<glm::vec3, 4> faceVertices = Geometry::GetFluidVertices(face);

        const glm::vec2 topLeftTexCoord = BlockAtlas::GetNormalizedTextureCoords(block, face);

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
          vertices.insert(vertices.end(), {
            pos + faceVertices[i],
            blockColor,
            faceTexCoords[i],
            normal
          });
        }

        indices.insert(indices.end(), {
          0, 1, 2,
          2, 3, 0,
        });
      }
    }

    void Chunk::AppendTranslucentFluidGeometry(BlockType block, glm::vec3 pos, std::vector<FaceGeometry> &translucentFaces) {
      const glm::vec3 blockPos = GetGlobalCoords(pos);

      for (int i = Geometry::Face::First; i != Geometry::Face::Last; ++i) {
        auto face = static_cast<Geometry::Face>(i);
        if (!IsFaceVisible(block, face, pos)) {
          continue;
        }

        FaceGeometry faceGeom;
        std::array<glm::vec3, 4> faceVertices = Geometry::GetFluidVertices(face);

        faceGeom.pos = blockPos + Utils::CalculateConvexCenter<4>(faceVertices);
        const glm::vec2 topLeftTexCoord = BlockAtlas::GetNormalizedTextureCoords(block, face);

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
            pos + faceVertices[i],
            blockColor,
            faceTexCoords[i],
            normal
          });
        }

        faceGeom.indices.insert(faceGeom.indices.end(), {
          0, 1, 2,
          2, 3, 0,
        });

        faceGeom.distanceToPlayer = glm::distance(m_world.GetPlayerPosition(), faceGeom.pos);

        translucentFaces.push_back(faceGeom);
      }
    }

    void Chunk::AppendFoliageGeometry(BlockType block, glm::vec3 pos, GLuint &indexOffset, std::vector<Geometry::MeshVertex> &vertices, std::vector<GLuint> &indices) {
      if (BlockData::Get(block).renderType != BlockRenderType::Foliage) {
        Utils::Logger::Warning("Appending foliage type for incorrect render type!");
        return;
      }

      const glm::vec3 blockPos = GetGlobalCoords(pos);


      const glm::vec4 blockColor { 0.0f };
      const glm::vec2 topLeftTexCoord = BlockAtlas::GetNormalizedTextureCoords(block, Geometry::Face::None);
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

      // faceGeom1.pos = blockPos + Utils::CalculateConvexCenter<4>(faceVertices1);
      // faceGeom2.pos = blockPos + Utils::CalculateConvexCenter<4>(faceVertices2);

      for (int i = 0; i < 4; ++i) {
        vertices.insert(vertices.end(), {
          pos + faceVertices1.at(i),
          blockColor,
          faceTexCoords[i],
          glm::vec3(0.0f)
        });
        vertices.insert(vertices.end(), {
          pos + faceVertices2.at(i),
          blockColor,
          faceTexCoords[i],
          glm::vec3(0.0f)
        });
      }

      // GLuint indexOffset = 0;

      indices.insert(indices.end(), {
        0, 1, 2,
        2, 3, 0,
      });

      indices.insert(indices.end(), {
        0, 1, 2,
        2, 3, 0,
      });

      // faceGeom1.distanceToPlayer = glm::distance(playerPos, faceGeom1.pos);
      // m_data.translucentFaces.push_back(faceGeom1);

      // faceGeom2.distanceToPlayer = glm::distance(playerPos, faceGeom2.pos);
      // m_data.translucentFaces.push_back(faceGeom2);
    }

  }

}
