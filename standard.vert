#version 460 core

layout(location = 0) in vec3 i_vertex_position;
layout(location = 1) in vec3 i_vertex_normal;
layout(location = 2) in vec2 i_vertex_uv;

uniform mat4 model_matrix;
uniform mat4 vp_matrix;

out vec3 vert_position;
out vec3 vert_normal;
out vec2 vert_uv;

void main()
{
    mat4 mvp = vp_matrix * model_matrix;
    gl_Position = mvp * vec4(i_vertex_position, 1.0);
    vert_position = vec3(model_matrix * vec4(i_vertex_position, 1.0));
    vert_normal = mat3(transpose(inverse(model_matrix))) * i_vertex_normal;
    vert_uv = i_vertex_uv;
}
