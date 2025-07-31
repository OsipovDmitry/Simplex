vec3 packNormal(in vec3 value)
{
    return 0.5f * value + vec3(0.5f);
}

vec3 unpackNormal(in vec3 value)
{
    return 2.0f * value - vec3(1.0f);
}