#version 460 core

in vec3 camera_direction;

uniform sampler2D u_environment_map;

out vec4 fragment_color;

vec2 sample_sphere_map(vec3 pt3d)
{
    const float inv_2_PI = 0.15915494309189533; // 1 / (2 * pi)
    const float inv_PI = 0.31830988618379067;   // 1 / (  pi)

    vec2 uv = vec2(atan(-pt3d.z, pt3d.x),
                   atan(pt3d.y, sqrt(pt3d.x * pt3d.x + pt3d.z * pt3d.z)));
    uv.x = 0.5 + uv.x * inv_2_PI;
    uv.y = 0.5 + uv.y * inv_PI;

    return vec2(uv.x, 1 - uv.y);
}

void main()
{
    vec3 direction = normalize(camera_direction);
    fragment_color = texture(u_environment_map, sample_sphere_map(direction));
}
