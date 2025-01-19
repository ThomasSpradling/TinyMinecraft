#version 330

in vec2 texCoord;
uniform sampler2D uBlockAtlas;

out vec4 FragColor;

void main()
{
  FragColor = texture(uBlockAtlas, texCoord);
}
