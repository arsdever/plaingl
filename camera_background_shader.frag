#version 460 core

in vec3 direction;

uniform sampler2D env_map;
uniform vec3 background_color;
uniform float use_color;

out vec4 fragment_color;

const float inv_2_PI = 0.15915494309189533;   // 1 / (2 * pi)
const float inv_PI = 0.31830988618379067;     // 1 / (  pi)

vec2 sample_sphere_map(vec3 pt3d)
{
    vec2 uv = vec2(atan(-pt3d.z, pt3d.x),
                   atan(pt3d.y, sqrt(pt3d.x * pt3d.x + pt3d.z * pt3d.z)));
    uv.x = 0.5 + uv.x * inv_2_PI;
    uv.y = 0.5 + uv.y * inv_PI;

    return vec2(uv.x, uv.y);
}

void main()
{
    vec3 dir = direction;
    dir = normalize(dir);
    fragment_color =
        texture(env_map, sample_sphere_map(dir)) * (1.0 - use_color) +
        vec4(background_color, 1.0) * use_color;
}
