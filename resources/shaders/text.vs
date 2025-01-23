#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTextureCoords;

uniform mat4 uProjection;

out vec3 fragColor;
out vec2 texCoords;

void main()
{
  gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
  fragColor = aColor;

  texCoords = aTextureCoords;
}