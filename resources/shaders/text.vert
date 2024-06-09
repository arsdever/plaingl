#version 460 core

layout(location = 0) in vec4 i_vertex_position;

uniform mat4 u_model_matrix;
uniform mat4 u_vp_matrix;

out vec2 fragment_uv;

void main()
{
    gl_Position =
        u_vp_matrix * u_model_matrix * vec4(i_vertex_position.xy, 0.0, 1.0);
    fragment_uv = i_vertex_position.zw;
}
