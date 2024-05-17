#version 460 core

in vec2 fragment_position;

uniform vec4 u_color;

out vec4 o_fragment_color;

void main() { o_fragment_color = u_color; }
