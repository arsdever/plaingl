#version 460 core

layout (location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_uv;

uniform mat4 model_matrix;
uniform mat4 vp_matrix;

out vec3 position;
out vec3 normal;
out vec2 uv;

void main()
{
    gl_Position = vp_matrix * model_matrix * vec4(vertex_position, 1.0);
    position = vertex_position;
    normal = vertex_normal;
    uv = vertex_uv;
}
