#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 uViewProjection;
uniform mat4 uModel;

out vec2 texCoord;

void main()
{
  gl_Position = uViewProjection * uModel * vec4(aPos, 1.0f);

  texCoord = aTexCoord;
}
