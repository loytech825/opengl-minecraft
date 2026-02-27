#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 transform;

void main()
{
    gl_Position = transform*vec4(aPos, 1.0);
    //outColor = vec3(0.5, 0.6, 0.2);
}