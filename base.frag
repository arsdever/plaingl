#version 460 core

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform sampler2D ambient_texture;

out vec4 fragment_color;

void main() { fragment_color = texture(ambient_texture, uv); }
