#version 460 core

uniform sampler2D texture_sampler;
uniform uint texture_type;

in vec2 uv_position;

out vec4 fragment_color;

void main()
{
    if (texture_type == 0)
    {
        fragment_color = vec4(0);
    }
    else if (texture_type == 1)
    {
        fragment_color = texture(texture_sampler, uv_position).xxxx;
    }
    else if (texture_type == 3)
    {
        fragment_color = vec4(texture(texture_sampler, uv_position).xxx, 1.0);
    }
    else
    {
        fragment_color = texture(texture_sampler, uv_position);
    }
}
