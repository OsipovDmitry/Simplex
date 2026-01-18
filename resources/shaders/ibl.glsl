vec3 fresnelSchlickRoughness(in float cosTheta, in vec3 F0, in float roughness)
{
    return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0f - cosTheta, 5.0f);
}

vec3 calculateIBL(
	in sampler2D IBLBRDFLutMap,
	in samplerCube IBLDiffuseMap,
	in samplerCube IBLSpecularMap,
	in vec3 baseColor,
	in float metalness,
	in float roughness,
	in vec3 F0,
	in vec3 N,
	in vec3 V)
{
    float NdotV = max(dot(N, V), 0.0f);
    vec3 R = normalize(reflect(-V, N));
    vec3 F = fresnelSchlickRoughness(NdotV, F0, roughness);
    
    vec3 kS = F;
    vec3 kD = (vec3(1.0f) - kS) * (1.0f - metalness);

    vec2 brdf = texture(IBLBRDFLutMap, vec2(NdotV, roughness)).rg;
    vec3 diffuse = baseColor * texture(IBLDiffuseMap, N).rgb;
    vec3 specular = (F * brdf.x + vec3(brdf.y)) * textureLod(IBLSpecularMap, R, roughness * float(textureQueryLevels(IBLSpecularMap))).rgb;

    return kD * diffuse + specular;
}

