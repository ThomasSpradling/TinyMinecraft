#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 uViewProjection;
uniform mat4 uModel;
uniform mat4 uLightViewProjection;

out vec2 texCoord;
out vec3 position;
out vec3 normal;
out vec4 lightSpacePos;

int BLOCK_TEXTURE_WIDTH = 32;
int BLOCK_TEXTURE_ATLAS_WIDTH = 32 * 5;
int BLOCK_TEXTURE_ATLAS_HEIGHT = 32 * 4;

void main()
{


  vec4 pos = uModel * vec4(aPos, 1.0f);
  position = vec3(pos);

  gl_Position  = uViewProjection * pos;
  texCoord = aTexCoord;
  normal = aNormal;

  lightSpacePos = uLightViewProjection * vec4(position, 1);
}
