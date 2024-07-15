#version 460 core

in vec3 fragment_position;
in vec3 fragment_normal;
in vec2 fragment_uv;
in vec3 fragment_tangent;
in vec3 fragment_bitangent;

uniform uint u_mode;
uniform vec3 u_camera_position;
uniform float u_wireframe;

const float PI = 3.14159265359;

struct light_t
{
    vec3 position;
    float intensity;
    vec3 direction;
    float radius;
    vec3 color;
    uint type;
};

out vec4 fragment_color;

vec2 confragment_from_blenders_uv_map(vec2 blender_uv)
{
    // blender's uv map coordinates range is from bottom-left
    // we need to convert them to top-left
    // (0,1) - (1,1)        (0,0) - (1,0)
    //   -       -     =>     -       -
    // (0,0) - (1,0)        (0,1) - (1,1)
    return vec2(fragment_uv.x, 1.0 - fragment_uv.y);
}

vec3 surface_normal(vec2 uv_coord) { return fragment_normal; }

vec3 calculate_light_ambient(light_t light)
{
    vec3 ambient = 0.1 * light.color;
    return ambient;
}

vec3 fresnel_schlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float ggx_distribution(vec3 vertex_normal, vec3 half_vector, float roughness)
{
    float roughness_2 = roughness * roughness;
    float numerator = roughness_2;

    float roughness_4 = roughness_2 * roughness_2;
    float reflection_angle_match_coefficient =
        max(dot(vertex_normal, half_vector), 0.0);
    float reflection_angle_match_coefficient_square =
        reflection_angle_match_coefficient * reflection_angle_match_coefficient;
    float denominator =
        (reflection_angle_match_coefficient * (roughness_2 - 1.0) + 1.0);
    denominator = PI * denominator * denominator;

    return numerator / denominator;
}

float ggx_geometry_schlick(float vector_match, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float numerator = vector_match;
    float denominator = vector_match * (1.0 - k) + k;

    return numerator / denominator;
}

float smiths_geometry_function(vec3 fragment_normal,
                               vec3 view_direction,
                               vec3 light_direction,
                               float roughness)
{
    float view_to_surface_direction_match =
        max(dot(fragment_normal, view_direction), 0.0);
    float light_to_surface_direction_match =
        max(dot(fragment_normal, light_direction), 0.0);
    float ggx2 =
        ggx_geometry_schlick(view_to_surface_direction_match, roughness);
    float ggx1 =
        ggx_geometry_schlick(light_to_surface_direction_match, roughness);

    return ggx1 * ggx2;
}

vec3 direct_light_contribution(light_t light,
                               vec3 surface_albedo,
                               vec3 surface_normal,
                               vec3 view_direction,
                               vec3 direct_fresnel,
                               float roughness,
                               float metallic)
{
    // calculate per-light radiance
    vec3 light_to_fragment_direction =
        normalize(light.position - fragment_position);
    vec3 half_vector = normalize(view_direction + light_to_fragment_direction);
    float light_to_fragment_distance =
        length(light.position - fragment_position);
    float attenuation = light.intensity / (light_to_fragment_distance *
                                           light_to_fragment_distance);
    vec3 radiance = light.color * attenuation;

    // cook-torrance brdf
    float normal_distribution =
        ggx_distribution(surface_normal, half_vector, roughness);
    float geometry_function = smiths_geometry_function(
        surface_normal, view_direction, light_to_fragment_direction, roughness);
    vec3 fresnel = fresnel_schlick(max(dot(half_vector, view_direction), 0.0),
                                   direct_fresnel);

    vec3 specular_light_coefficient = fresnel;
    vec3 direct_light_coefficient = vec3(1.0) - specular_light_coefficient;
    direct_light_coefficient *= 1.0 - metallic;

    vec3 numerator = normal_distribution * geometry_function * fresnel;
    float denominator =
        4.0 * max(dot(surface_normal, view_direction), 0.0) *
            max(dot(surface_normal, light_to_fragment_direction), 0.0) +
        0.0001;
    vec3 specular = numerator / denominator;

    // add to outgoing radiance Lo
    float NdotL = max(dot(surface_normal, light_to_fragment_direction), 0.0);

    return (direct_light_coefficient * surface_albedo / PI + specular) *
           radiance * NdotL;
}

void main()
{
    light_t[ 2 ] lights;
    lights[ 0 ].position = vec3(3, 3, 3);
    lights[ 0 ].color = vec3(1);
    lights[ 0 ].intensity = 15;
    lights[ 1 ].position = vec3(-3, -3, 3);
    lights[ 1 ].color = vec3(1);
    lights[ 1 ].intensity = 10;

    vec3 surface_normal = surface_normal(fragment_uv);
    vec3 view_direction = normalize(u_camera_position - fragment_position);

    vec3 direct_fresnel = vec3(0.04);
    direct_fresnel = mix(direct_fresnel, vec3(1), 0);

    vec3 color;
    if (u_mode == 0)
    {
        // standard shading
        color = vec3(1);
    }
    else if (u_mode == 1)
    {
        // normals
        color = (surface_normal + 1) / 2;
    }
    else if (u_mode == 2)
    {
        // tangents
        color = (fragment_tangent + 1) / 2;
    }
    else if (u_mode == 3)
    {
        // tangents
        color = (fragment_bitangent + 1) / 2;
    }

    // reflectance equation
    vec3 direct_light = vec3(0.0);
    for (int i = 0; i < lights.length(); ++i)
    {
        direct_light += direct_light_contribution(lights[ i ],
                                                  color,
                                                  surface_normal,
                                                  view_direction,
                                                  direct_fresnel,
                                                  .2,
                                                  0);
    }

    vec3 ambient = vec3(0.3) * color * .5;
    color = ambient + direct_light;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    color *= (1 - u_wireframe);
    fragment_color = vec4(color, 1.0);
}
