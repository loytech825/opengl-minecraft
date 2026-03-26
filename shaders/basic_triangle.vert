#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 texturePos;

out vec2 textureCoords;

uniform mat4 transform;

void main()
{
    gl_Position = transform*vec4(aPos, 1.0);
    textureCoords = texturePos;
    //outColor = vec3(0.5, 0.6, 0.2);
}