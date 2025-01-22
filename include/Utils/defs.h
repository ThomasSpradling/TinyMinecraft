#ifndef DEF_H_
#define DEF_H_

#define BLOCK_TEXTURE_WIDTH 32
#define BLOCK_TEXTURE_ATLAS_WIDTH (32 * 5)
#define BLOCK_TEXTURE_ATLAS_HEIGHT (32 * 4)

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 256
#define CHUNK_LENGTH 16

#define FIXED_UPDATE_INTERVAL (1000.0f / 60.0f)

// Perf
  // #define UTILS_ShowFPS
  #define UTILS_RunProfile
  // #define UTILS_ProfileVerbose    // Will print profile data on every run

#define GAMEPLAY_MaxBlockInteractDistance (5.0f)

//// Mesh method
  #define MESH_METHOD_Naive
  // #define MESH_METHOD_VertexHiding
  // #define MESH_METHOD_Serialize
  // #define MESH_METHOD_Greedy2D

#endif // DEF_H_
