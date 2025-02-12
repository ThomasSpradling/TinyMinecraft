#version 330 core

in vec4 v_Color;

out vec4 FragColor;

void main() {
  FragColor = v_Color;

  gl_FragDepth = gl_FragCoord.z - 0.00001; // bias wireframes when overlapping
}
