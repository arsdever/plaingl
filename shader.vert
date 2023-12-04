#version 460 core

layout (location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;

uniform mat4 mvp_matrix;

out vec3 position;
out vec3 normal;

void main()
{
    gl_Position = mvp_matrix * vec4(vertexPosition, 1.0);
    position = vertexPosition;
    normal = vertexNormal;
}
