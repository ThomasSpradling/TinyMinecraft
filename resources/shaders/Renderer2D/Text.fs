#version 330 core

in vec4 v_FragColor;
in vec2 v_TexCoords;

out vec4 FragColor;

uniform sampler2D u_FontMap;

void main()
{
  vec4 color = texture(u_FontMap, v_TexCoords);

  const vec3 magenta = vec3(1.0, 0.0, 1.0);
  const float threshold = 0.01;

  if (distance(color.rgb, magenta) < threshold) {
    discard;
  }

  color.xyz += v_FragColor.xyz;
  FragColor = color;
}