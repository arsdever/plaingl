#version 460 core

in vec2 fragment_uv;

out vec4 o_fragment_color;

uniform sampler2D u_text;
uniform vec3 u_text_color;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_text, fragment_uv).r);
    o_fragment_color = vec4(u_text_color, 1.0) * sampled;
}
