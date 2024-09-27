#version 460 core

in vec2 fragment_uv;

uniform vec4 u_color = vec4(1);
uniform sampler2D u_image;
uniform sampler2D u_mask;

out vec4 o_fragment_color;

void main()
{
    o_fragment_color = u_color * texture(u_image, fragment_uv) *
                       texture(u_mask, fragment_uv).r;
}
