#version 460 core

// expected coordinates from (-1,-1) to (1,1)
layout(location = 0) in vec3 i_vertex_position;

uniform mat4 u_model_matrix;
uniform mat4 u_vp_matrix;

out vec2 fragment_uv;
out vec3 fragment_position;

void main()
{
    gl_Position = u_vp_matrix * u_model_matrix * vec4(i_vertex_position, 1.0);
    fragment_position = i_vertex_position;
}
