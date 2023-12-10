#version 460 core

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform sampler2D ambient_texture;
uniform vec4 ambient_color;
uniform sampler2D normal_texture;
uniform vec3 light_pos;
uniform vec3 light_color;
uniform float light_intensity;

out vec4 fragment_color;

void main()
{
    vec3 norm = texture(normal_texture, uv).rgb;
    norm = normalize(norm * 2 - 1);
    vec3 light_dir = normalize(light_pos - position);
    float diff = max(dot(norm, light_dir), 0.0) + .2;
    vec3 diffuse = diff * light_color * light_intensity;

    ambient_color;
    fragment_color = vec4(diffuse, 1) * texture(ambient_texture, uv);
}
