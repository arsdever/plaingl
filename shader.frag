#version 460 core

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform sampler2D ambient_texture;
uniform float ambient_texture_strength;
uniform vec4 ambient_color;
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

vec4 ambient_mixed_color(vec2 uv_coord)
{
    return ambient_color * (1 - ambient_texture_strength) +
           texture(ambient_texture, uv_coord) * ambient_texture_strength;
}

void main()
{
    vec2 converted_uv = convert_from_blenders_uv_map(uv);
    vec3 norm = texture(normal_texture, converted_uv).rgb;
    norm = normalize(norm * 2 - 1);
    norm = mix(norm, normal, normal_texture_strength);
    vec3 light_dir = normalize(light_pos - position);
    float diff = max(dot(norm, light_dir), 0.0) + .2;
    vec3 diffuse = diff * light_color * light_intensity;

    ambient_color;
    fragment_color = vec4(diffuse, 1) * ambient_mixed_color(converted_uv);
}
