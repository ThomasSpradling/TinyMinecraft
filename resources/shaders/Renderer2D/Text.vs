#version 330 core

layout (location = 0) in vec2 a_Pos;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_TexCoords;

uniform mat4 u_ViewProjection;

out vec4 v_FragColor;
out vec2 v_TexCoords;

void main() {
  gl_Position = u_ViewProjection * vec4(a_Pos, 0, 1);

  v_FragColor = a_Color;
  v_TexCoords = a_TexCoords;
}
