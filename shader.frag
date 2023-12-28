#version 460 core

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform sampler2D albedo_texture;
uniform float albedo_texture_strength;
uniform vec4 albedo_color;
uniform sampler2D normal_texture;
uniform float normal_texture_strength;
uniform vec3 light_pos;
uniform vec3 light_color;
uniform float light_intensity;

out vec4 fragment_color;

vec2 convert_from_blenders_uv_map(vec2 blender_uv)
{
    // blender's uv map coordinates range is from bottom-left
    // we need to convert them to top-left
    // (0,1) - (1,1)        (0,0) - (1,0)
    //   -       -     =>     -       -
    // (0,0) - (1,0)        (0,1) - (1,1)
    return vec2(uv.x, 1.0 - uv.y);
}

vec4 albedo_mixed_color(vec2 uv_coord)
{
    return albedo_color * (1 - albedo_texture_strength) +
           texture(albedo_texture, uv_coord) * albedo_texture_strength;
}

void main()
{
    vec2 converted_uv = convert_from_blenders_uv_map(uv);
    // vec2 converted_uv = uv;
    // vec3 norm = texture(normal_texture, converted_uv).rgb;
    // norm = normalize(norm * 2 - 1);
    // norm = mix(norm, normal, normal_texture_strength);
    vec3 norm = normal;

    // vec3 light_dir = normalize(light_pos - position);
    // vec3 view_dir = normalize(view_pos - position);
    // vec3 halfway_dir = normalize(light_dir + view_dir);

    vec3 ambient = 0.2 * light_color;
    vec3 light_dir = normalize(light_pos - position);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light_color * light_intensity;

    vec3 result_color =
        (diffuse + ambient) * albedo_mixed_color(converted_uv).rgb;
    fragment_color = vec4(result_color, 1.0);
}
