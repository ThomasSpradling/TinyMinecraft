#version 330 core

in vec3 fragColor;
in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D uFontMap;

void main()
{
  vec4 color = texture(uFontMap, texCoords);

  const vec3 magenta = vec3(1.0, 0.0, 1.0);
  const float threshold = 0.01;

  if (distance(color.rgb, magenta) < threshold) {
    discard;
  }

  color.xyz += fragColor;
  FragColor = color;
}