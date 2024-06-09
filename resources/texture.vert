#version 460 core

layout(location = 0) in vec2 vertex_position;

out vec2 uv_position;

void main()
{
    vec2 position = vertex_position.xy * 2 - 1;
    position.y = -position.y;
    gl_Position = vec4(position, 0.0, 1.0);
    uv_position = vertex_position;
}
