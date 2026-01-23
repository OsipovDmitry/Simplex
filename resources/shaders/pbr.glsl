#include<math/constants.glsl>

float DistributionGGX(in float NdotH, in float roughness)
{
    float a2 = roughness * roughness * roughness * roughness;

    float num   = a2;
    float denom = (NdotH * NdotH * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(in float NdotV, in float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;

    float num   = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return num / denom;
}

vec3 fresnelSchlick(in float cosTheta, in vec3 F0)
{
    return F0 + (vec3(1.0f) - F0) * pow(1.0f - cosTheta, 5.0f);
}

vec3 calculatePBR(
	in vec3 baseColor,
	in float metalness,
	in float roughness,
	in vec3 F0,
	in vec3 lightColor,
	in vec3 N,
	in vec3 L,
	in vec3 V)
{
    vec3 H = normalize(L + V);
    float NdotL = max(dot(N, L), 0.0f);
    float NdotV = max(dot(N, V), 0.0f);
    float NdotH = max(dot(N, H), 0.0f);
    float HdotV = max(dot(H, V), 0.0f);

    float NDF = DistributionGGX(NdotH, roughness);
    float G = GeometrySchlickGGX(NdotL, roughness) * GeometrySchlickGGX(NdotV, roughness);
    vec3 F = fresnelSchlick(HdotV, F0);

    vec3 kS = F;
    vec3 kD = (vec3(1.0f) - kS) * (1.0f - metalness);

    vec3 specular = (NDF * G * F) / max((4.0f * NdotV * NdotL), 1e-3f);

    return lightColor * NdotL * (kD * baseColor / PI + specular);
}

