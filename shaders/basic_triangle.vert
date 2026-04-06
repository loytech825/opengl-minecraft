#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 texturePos;
layout (location = 2) in float texture_id;

out vec2 textureCoords;
out float texID;

uniform mat4 transform;

void main()
{
    gl_Position = transform*vec4(aPos, 1.0);
    textureCoords = texturePos;
    texID = texture_id;
    //outColor = vec3(0.5, 0.6, 0.2);
}