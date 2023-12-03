#version 460 core

uniform uint object_index;

layout(location = 0) out vec4 diffuseColor;
layout(location = 1) out uvec3 fragmentColor;

void main()
{
    diffuseColor = vec4(object_index, 0.0f, 0.0f, 1.0f);
    fragmentColor = uvec3(object_index, 0, 0);
}
