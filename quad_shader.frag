#version 460 core

in vec2 _vertex_uv;

uniform sampler2D _i_input_image;

out vec4 _o_fragment_color;

void main() { _o_fragment_color = texture(_i_input_image, _vertex_uv); }
