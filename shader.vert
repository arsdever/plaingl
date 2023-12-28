#version 460 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_uv;

uniform mat4 model_matrix;
uniform mat4 vp_matrix;

out vec3 position;
out vec3 normal;
out vec2 uv;

void main()
{
    mat4 mvp = vp_matrix * model_matrix;
    gl_Position = mvp * vec4(vertex_position, 1.0);
    position = vec3(model_matrix * vec4(vertex_position, 1.0));
    normal = mat3(transpose(inverse(model_matrix))) * vertex_normal;
    uv = vertex_uv;
}
