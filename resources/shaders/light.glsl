#include<math/transform.glsl>
#include<descriptions.glsl>

const uint PointLightTypeID = 0u;
const uint SpotLightTypeID = 1u;
const uint DirectionalLightTypeID = 2u;
const uint ImageBasedLightTypeID = 3u;
const uint EmptyLightTypeID = 0xFFFFFFFFu;

layout (std430) readonly buffer ssbo_lightsBuffer { LightDescription lights[]; };

Transform lightTransform(in uint lightID)
{
	return makeTransform(lights[lightID].transform);
}

uint lightTypeID(in uint lightID)
{
	return floatBitsToUint(lights[lightID].flags0.w);
}

vec3 pointLightColor(in uint lightID)
{
	return lights[lightID].flags0.xyz;
}

vec2 pointLightRadiuses(in uint lightID)
{
	return lights[lightID].flags1.xy;
}

vec3 spotLightColor(in uint lightID)
{
	return lights[lightID].flags0.xyz;
}

vec2 spotLightRadiuses(in uint lightID)
{
	return lights[lightID].flags1.xy;
}

vec2 spotLightHalfAngles(in uint lightID)
{
	return lights[lightID].flags1.zw;
}

vec3 directionalLightColor(in uint lightID)
{
	return lights[lightID].flags0.xyz;
}

uint imageBasedLightBRDFLutMapID(in uint lightID)
{
	return floatBitsToUint(lights[lightID].flags1.x);
}

uint imageBasedLightDiffuseMapID(in uint lightID)
{
	return floatBitsToUint(lights[lightID].flags1.y);
}

uint imageBasedLightSpecularMapID(in uint lightID)
{
	return floatBitsToUint(lights[lightID].flags1.z);
}

float imageBasedLightContribution(in uint lightID)
{
	return lights[lightID].flags1.w;
}