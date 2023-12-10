#version 460 core

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform sampler2D ambient_texture;
uniform vec4 ambient_color;
uniform sampler2D normal_texture;

out vec4 fragment_color;

vec3 lightPos;
vec3 lightColor;

void main()
{
    lightPos = vec3(0, 10, 5);
    lightColor = vec3(1, 1, 1);
    vec3 norm = texture(normal_texture, uv).rgb;
    norm = normalize(norm * 2 - 1);
    vec3 lightDir = normalize(lightPos - position);
    float diff = max(dot(norm, lightDir), 0.0) + .2;
    vec3 diffuse = diff * lightColor;

    ambient_color;
    fragment_color = vec4(diffuse, 1) * texture(ambient_texture, uv);
}
