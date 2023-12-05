#version 460 core

uniform sampler2D texture_sampler;

in vec2 uv_position;

out vec4 fragment_color;

void main() { fragment_color = texture(texture_sampler, uv_position); }
