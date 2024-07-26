#version 460 core

in vec2 fragment_uv;

out vec4 o_fragment_color;

uniform vec4 u_color = vec4(1);
uniform sampler2D u_image;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_image, fragment_uv).r);
    o_fragment_color = u_color * sampled;
}
