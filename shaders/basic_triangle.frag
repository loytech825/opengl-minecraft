#version 330 core

out vec4 FragColor;

in vec3 outColor;
in vec2 textureCoords;
uniform sampler2D stone_texture;

void main()
{
    FragColor = texture(stone_texture, textureCoords);
}