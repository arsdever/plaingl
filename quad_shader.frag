#version 460 core

in vec2 fragment_uv;

uniform sampler2D i_input_image;

out vec4 o_fragment_color;

void main() { o_fragment_color = texture(i_input_image, fragment_uv); }
