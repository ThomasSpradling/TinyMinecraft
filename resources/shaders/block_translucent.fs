#version 330

in vec3 position;
in vec2 texCoord;

uniform sampler2D uBlockAtlas;

out vec4 FragColor;

void main()
{
  vec4 texColor = texture(uBlockAtlas, texCoord);
  if (texColor.a == 0)
    discard;

  FragColor = texColor;
}
