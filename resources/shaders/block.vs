#version 330 core

layout (location = 0) in int aMask;

uniform mat4 uViewProjection;
uniform mat4 uModel;

out vec2 texCoord;
out vec3 position;
out vec3 normal;

int BLOCK_TEXTURE_WIDTH = 32;
int BLOCK_TEXTURE_ATLAS_WIDTH = 32 * 5;
int BLOCK_TEXTURE_ATLAS_HEIGHT = 32 * 4;

vec3 GetNormal(int id) {
  switch (id) {
    case 0:
      return vec3(1, 0, 0);
    case 1:
      return vec3(-1, 0, 0);
    case 2:
      return vec3(0, 1, 0);
    case 3:
      return vec3(0, -1, 0);
    case 4:
      return vec3(0, 0, 1);
    case 5:
      return vec3(0, 0, -1);
    default:
      return vec3(0);
  }
}

void main()
{
  float x = float(aMask & ((1 << 5) - 1));
  float y = float((aMask >> 5) & ((1 << 9) - 1)); 
  float z = float((aMask >> 14) & ((1 << 5) - 1)); 

  int normalId = int((aMask >> 19) & ((1 << 3) - 1)); 
  float u = float((aMask >> 23) & ((1 << 3) - 1));        
  float v = float((aMask >> 26) & ((1 << 3) - 1));        

  vec3 aPos = vec3(x, y, z);

  vec2 texSize = vec2(BLOCK_TEXTURE_WIDTH / float(BLOCK_TEXTURE_ATLAS_WIDTH), BLOCK_TEXTURE_WIDTH / float(BLOCK_TEXTURE_ATLAS_HEIGHT));
  texCoord = vec2(u, v) * texSize;

  gl_Position = uViewProjection * uModel * vec4(aPos, 1.0f);

  position = vec3(gl_Position);
  normal = GetNormal(normalId);

}
