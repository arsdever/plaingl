#version 460 core

uniform uint object_id;

layout(location = 0) out vec4 diffuseColor;
layout(location = 1) out uvec3 fragmentColor;

float hueToRgb(float p, float q, float t)
{
    if (t < 0.0f)
        t += 1.0f;
    if (t > 1.0f)
        t -= 1.0f;
    if (t < 1.0f / 6.0f)
        return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f)
        return q;
    if (t < 2.0f / 3.0f)
        return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
}

vec3 hslToRgb(float h, float s, float l)
{
    if (s == 0.0f)
    {
        return vec3(1, 1, 1);
    }
    else
    {
        const float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
        const float p = 2.0f * l - q;
        float r = hueToRgb(p, q, h + 1.0f / 3.0f);
        float g = hueToRgb(p, q, h);
        float b = hueToRgb(p, q, h - 1.0f / 3.0f);
        return vec3(r, g, b);
    }
}

void main()
{
    diffuseColor = vec4(hslToRgb(object_id / 2.0, 1.0f, 0.5f), 1.0f);
    fragmentColor = uvec3(object_id, 0, 0);
}
