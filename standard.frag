#version 460 core

in vec3 vert_position;
in vec3 vert_normal;
in vec2 vert_uv;

uniform sampler2D albedo_texture;
uniform float albedo_texture_strength;
uniform vec4 albedo_color;
uniform sampler2D normal_texture;
uniform float normal_texture_strength;
uniform float u_metallic;
uniform float u_roughness;
uniform float u_ao;

uniform vec3 u_camera_position;

#define LIGHT_OMNI  0
#define SPOTLIGHT   1
#define SPHERICAL   2
#define DIRECTIONAL 3
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

layout(std430, binding = 0) buffer lights_buffer { light_t[] lights; };

out vec4 fragment_color;

vec2 convert_from_blenders_uv_map(vec2 blender_uv)
{
    // blender's uv map coordinates range is from bottom-left
    // we need to convert them to top-left
    // (0,1) - (1,1)        (0,0) - (1,0)
    //   -       -     =>     -       -
    // (0,0) - (1,0)        (0,1) - (1,1)
    return vec2(vert_uv.x, 1.0 - vert_uv.y);
}

vec4 albedo_mixed_color(vec2 uv_coord)
{
    return mix(albedo_color,
               texture(albedo_texture, uv_coord),
               albedo_texture_strength);
}

vec3 calculate_light_ambient(light_t light)
{
    vec3 ambient = 0.1 * light.color;
    return ambient;
}

vec3 calculate_light_diffuse(light_t light, vec3 fragment_normal)
{
    vec3 light_dir = normalize(light.position - vert_position);
    float diff = max(dot(fragment_normal, light_dir), 0.0);

    float dist = distance(light.position, vert_position);
    float intensity = light.intensity / (dist * dist);
    vec3 diffuse = diff * light.color * intensity;

    return diffuse;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

void main()
{
    vec3 N = normalize(vert_normal);
    vec3 V = normalize(u_camera_position - vert_position);

    vec3 F0 = vec3(0.04);
    vec3 albedo = albedo_mixed_color(vert_uv).xyz;
    F0 = mix(F0, albedo.rgb, u_metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < lights.length(); ++i)
    {
        light_t l = lights[ i ];
        // calculate per-light radiance
        vec3 L = normalize(l.position - vert_position);
        vec3 H = normalize(V + L);
        float distance = length(l.position - vert_position);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = l.color * attenuation;

        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, u_roughness);
        float G = GeometrySmith(N, V, L, u_roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - u_metallic;

        vec3 numerator = NDF * G * F;
        float denominator =
            4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * albedo * u_ao;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    fragment_color = vec4(color, 1.0);
}
