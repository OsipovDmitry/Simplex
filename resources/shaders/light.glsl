#include<descriptions.glsl>

#include<math/transform.glsl>

const uint PointLightTypeID = 0u;
const uint SpotLightTypeID = 1u;
const uint DirectionalLightTypeID = 2u;
const uint ImageBasedLightTypeID = 3u;
const uint AmbientLightTypeID = 4u;
const uint UndefinedLightTypeID = 0xFFFFFFFFu;

layout (std430) readonly buffer ssbo_lightsBuffer { LightDescription lights[]; };

Transform lightTransform(in uint lightID)
{
	return toTransform(lights[lightID].transform);
}

bool lightIsEnabled(in uint lightID)
{
	return bitfieldExtract(lights[lightID].flags, 0, 1) != 0u;
}

uint lightTypeID(in uint lightID)
{
	return floatBitsToUint(lights[lightID].params0.w);
}

uint lightShadowID(in uint lightID)
{
	return lights[lightID].shadowID;
}

vec3 pointLightColor(in uint lightID)
{
	return lights[lightID].params0.xyz;
}

vec2 pointLightRadiuses(in uint lightID)
{
	return lights[lightID].params1.xy;
}

vec3 spotLightColor(in uint lightID)
{
	return lights[lightID].params0.xyz;
}

vec2 spotLightRadiuses(in uint lightID)
{
	return lights[lightID].params1.xy;
}

vec2 spotLightHalfAngles(in uint lightID)
{
	return lights[lightID].params1.zw;
}

vec3 directionalLightColor(in uint lightID)
{
	return lights[lightID].params0.xyz;
}

uint directionalLightShadowCascadesCount(in uint lightID)
{
	return floatBitsToUint(lights[lightID].params1.x);
}

uint imageBasedLightBRDFLutMapID(in uint lightID)
{
	return floatBitsToUint(lights[lightID].params1.x);
}

uint imageBasedLightDiffuseMapID(in uint lightID)
{
	return floatBitsToUint(lights[lightID].params1.y);
}

uint imageBasedLightSpecularMapID(in uint lightID)
{
	return floatBitsToUint(lights[lightID].params1.z);
}

float imageBasedLightContribution(in uint lightID)
{
	return lights[lightID].params1.w;
}

vec3 ambientLightColor(in uint lightID)
{
	return lights[lightID].params0.xyz;
}