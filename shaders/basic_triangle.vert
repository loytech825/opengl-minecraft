#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 texturePos;
layout (location = 2) in float texture_id;
layout (location = 3) in float normal_dir;

out vec2 textureCoords;
out float texID;
out vec3 normal;

uniform mat4 transform;

vec3[6] normals = {
    vec3(0, 1, 0),
    vec3(1, 0, 0),
    vec3(0, 0, 1),
    vec3(0, 0, -1),
    vec3(-1, 0, 0),
    vec3(0, -1, 0)
};


void main()
{
    gl_Position = transform*vec4(aPos, 1.0);
    textureCoords = texturePos;
    texID = texture_id;
    normal = normals[int(normal_dir)];
    //outColor = vec3(0.5, 0.6, 0.2);
}