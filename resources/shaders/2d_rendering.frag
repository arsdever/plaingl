#version 460 core

in vec2 fragment_position;
in vec4 fragment_color;

out vec4 o_fragment_color;

void main() { o_fragment_color = fragment_color; }
