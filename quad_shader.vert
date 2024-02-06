#version 460 core

// expected coordinates from (-1,-1) to (1,1)
layout(location = 0) in vec3 _i_vertex_position;
// expected coordinates from (0,0) to (1,1)
layout(location = 2) in vec2 _i_vertex_uv;

out vec2 _vertex_uv;

void main()
{
    gl_Position = vec4(_i_vertex_position, 1.0);
    _vertex_uv = (_i_vertex_uv * 0.5) + 0.5;
}
