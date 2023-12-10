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

vec2 convert_from_blenders_uv_map(vec2 blender_uv)
{
    // blender's uv map coordinates range is from bottom-left
    // we need to convert them to top-left
    // (0,1) - (1,1)        (0,0) - (1,0)
    //   -       -     =>     -       -
    // (0,0) - (1,0)        (0,1) - (1,1)
    return vec2(uv.x, 1.0 - uv.y);
}

void main()
{
    vec2 converted_uv = convert_from_blenders_uv_map(uv);
    vec3 norm = texture(normal_texture, converted_uv).rgb;
    norm = normalize(norm * 2 - 1);
    vec3 light_dir = normalize(light_pos - position);
    float normal_strength = 0.2;
    float diff = max(1 - normal_strength * dot(norm, light_dir), 0.0) + .2;
    vec3 diffuse = diff * light_color * light_intensity;

    ambient_color;
    fragment_color = vec4(diffuse, 1) * texture(ambient_texture, converted_uv);
}
