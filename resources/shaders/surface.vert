#version 460 core

// expected coordinates from (-1,-1) to (1,1)
layout(location = 0) in vec2 i_vertex_position;
// expected coordinates from (0,0) to (1,1)
layout(location = 1) in vec2 i_vertex_uv;

out vec2 fragment_uv;

void main()
{
    gl_Position = vec4(i_vertex_position, 0.0, 1.0);
    fragment_uv = vec2(i_vertex_uv.x, 1 - i_vertex_uv.y);
}
