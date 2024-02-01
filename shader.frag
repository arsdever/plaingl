#version 460 core

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform sampler2D albedo_texture;
uniform float albedo_texture_strength;
uniform vec4 albedo_color;
uniform sampler2D normal_texture;
uniform float normal_texture_strength;

#define LIGHT_OMNI  0
#define SPOTLIGHT   1
#define SPHERICAL   2
#define DIRECTIONAL 3

struct light_t
{
    vec3 position;
    float intensity;
    vec3 direction;
    float radius;
    vec3 color;
    uint type;
};

layout(std430, binding = 0) buffer lights_buffer { light_t[] lights; };

out vec4 fragment_color;
out vec4 render_texture_fragment_color;

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

vec3 calculate_light_ambient(light_t light)
{
    vec3 ambient = 0.2 * light.color;
    return ambient;
}

vec3 calculate_light_diffuse(light_t light, vec3 fragment_normal)
{
    vec3 light_dir = normalize(light.position - position);
    float diff = max(dot(fragment_normal, light_dir), 0.0);
    vec3 diffuse = diff * light.color * light.intensity;

    return diffuse;
}

void main()
{
    vec2 converted_uv = convert_from_blenders_uv_map(uv);
    vec3 norm = texture(normal_texture, converted_uv).rgb;
    norm = normalize(norm * 2 - 1);
    norm = mix(norm, normal, normal_texture_strength);

    vec3 combined_diffuse = vec3(0, 0, 0);
    vec3 combined_ambient = vec3(0, 0, 0);

    for (int i = 0; i < lights.length(); ++i)
    {
        combined_diffuse += calculate_light_diffuse(lights[ i ], norm);
        combined_ambient += calculate_light_ambient(lights[ i ]);
    }

    vec3 result_color = (combined_diffuse + combined_ambient) *
                        albedo_mixed_color(converted_uv).rgb;
    fragment_color = vec4(result_color, 1.0);
    render_texture_fragment_color = fragment_color;
}
