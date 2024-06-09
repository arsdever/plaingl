#version 460 core

uniform vec4 u_color;

out vec4 o_fragment_color;

void main() { o_fragment_color = u_color; }
