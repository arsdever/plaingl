#version 460 core

layout(location = 0) in vec2 i_vertex_position;

uniform mat4 u_camera_matrix;

out vec3 camera_direction;

void main()
{
    gl_Position = vec4(i_vertex_position, 0.9999, 1.0);
    camera_direction =
        (u_camera_matrix *
         vec4(i_vertex_position.x, i_vertex_position.y, -1.0, 1.0))
            .xyz;
}
