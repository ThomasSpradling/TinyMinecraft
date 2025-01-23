#version 330 core

in vec3 fragColor;
in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D uFontMap;

void main()
{
  vec4 color = texture(uFontMap, texCoords);
  // color.xyz += fragColor;

  FragColor = color;
}