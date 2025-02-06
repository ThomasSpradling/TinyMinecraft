#ifndef DEF_H_
#define DEF_H_

#define __DEBUG__

// Graphics

// Shadow mapping (BROKEN!)
  // #define GFX_ShadowMapping

// Values
  #define GFX_RENDER_DISTANCE 12

// text
#define TEXT_CHAR_WIDTH 12
#define TEXT_CHAR_HEIGHT 16
#define FONT_MAP_WIDTH (TEXT_CHAR_WIDTH * 8)
#define FONT_MAP_HEIGHT (TEXT_CHAR_HEIGHT * 8)

#define BLOCK_TEXTURE_WIDTH 32
#define BLOCK_TEXTURE_ATLAS_WIDTH (32 * 5)
#define BLOCK_TEXTURE_ATLAS_HEIGHT (32 * 4)

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 256
#define CHUNK_LENGTH 16

#define FIXED_UPDATE_INTERVAL (1000.0f / 60.0f)

// Perf
  #define UTILS_ShowFPS
  // #define UTILS_RunProfile
  // #define UTILS_ProfileVerbose    // Will print profile data on every run

#define GAMEPLAY_MaxBlockInteractDistance (5.0f)

#endif // DEF_H_
