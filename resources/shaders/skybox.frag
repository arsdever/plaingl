#version 460 core

in vec2 fragment_uv;
in vec3 fragment_position;

out vec4 o_fragment_color;

void main()
{
    const float PI = 3.141592;
    const vec4 ground_color = vec4(0.4235, 0.3294, 0.2314, 1.0);
    const vec4 horizon_color = vec4(1.0, 1.0, 1.0, 1.0);
    const vec4 sky_color = vec4(0.9176, 0.9569, 0.9725, 1.0);

    const float fragment_angle =
        acos(dot(normalize(fragment_position), vec3(0.0, 1.0, 0.0)));

    const vec4 ground_horizon_mix =
        mix(sky_color,
            ground_color,
            min(1.0, max(0.0, (fragment_angle - PI / 2) / PI * 1000)));

    o_fragment_color = ground_horizon_mix;
}
