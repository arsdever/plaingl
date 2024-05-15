#version 460 core

layout(location = 0) in vec2 i_vertex_position;
layout(location = 1) in vec4 i_vertex_color;

out vec2 fragment_position;
out vec4 fragment_color;

void main()
{
    fragment_position = i_vertex_position;
    fragment_color = i_vertex_color;
    gl_Position = vec4(i_vertex_position, 0, 1);
}
