#version 330

layout(location = 0) in vec3 i_vertex_position;

uniform mat4 u_model_matrix;
uniform mat4 u_vp_matrix;

void main()
{
    mat4 mvp = u_vp_matrix * u_model_matrix;
    gl_Position = mvp * vec4(i_vertex_position, 1.0);
}
