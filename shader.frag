#version 460 core

in vec3 position;
in vec3 normal;

uniform vec4 materialColor;
uniform int is_selected;

vec3 lightPos;
vec3 lightColor;

out vec4 fragmentColor;

void main()
{
    lightPos = vec3(0, 10, 5);
    lightColor = vec3(1, 1, 1);
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    fragmentColor = vec4(diffuse, 1) * materialColor + vec4(is_selected);
}
