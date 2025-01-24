#version 330 core
layout (location = 0) in int aMask;

uniform mat4 uLightViewProjection;
uniform mat4 uModel;

void main() {
  float x = float(aMask & ((1 << 5) - 1));
  float y = float((aMask >> 5) & ((1 << 9) - 1)); 
  float z = float((aMask >> 14) & ((1 << 5) - 1)); 
  vec3 aPos = vec3(x, y, z);

  gl_Position = uLightViewProjection * uModel * vec4(aPos, 1.0);
}
