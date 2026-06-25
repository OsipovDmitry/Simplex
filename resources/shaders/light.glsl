#include<descriptions.glsl>

#include<math/transform.glsl>
#include<math/range.glsl>

const uint AmbientLightTypeID = 0u;
const uint DirectionalLightTypeID = 1u;
const uint PointLightTypeID = 2u;
const uint SpotLightTypeID = 3u;
const uint ImageBasedLightTypeID = 4u;
const uint UndefinedLightTypeID = 0xFFFFFFFFu;

#define IsLightTypeDefined(typeID) \
	((typeID == AmbientLightTypeID) || \
	 (typeID == DirectionalLightTypeID) || \
	 (typeID == PointLightTypeID) || \
	 (typeID == SpotLightTypeID) || \
	 (typeID == ImageBasedLightTypeID))

#define IsLightTypeColored(typeID) \
	((typeID == AmbientLightTypeID) || \
	 (typeID == DirectionalLightTypeID) || \
	 (typeID == PointLightTypeID) || \
	 (typeID == SpotLightTypeID))

#define IsLightTypeShadowed(typeID) \
	((typeID == DirectionalLightTypeID) || \
	 (typeID == PointLightTypeID) || \
	 (typeID == SpotLightTypeID))

layout (std430) readonly buffer ssbo_lightsBuffer { LightDescription lights[]; };

Transform lightTransform(in uint lightID)
{
	return toTransform(lights[lightID].transform);
}

uint lightTypeID(in uint lightID)
{
	return lights[lightID].params0[0u];
}

bool lightIsLightingEnabled(in uint lightID)
{
	return bitfieldExtract(lights[lightID].params2[0u], 0, 1) != 0u;
}

vec3 coloredLightColor(in uint lightID)
{
	return uintBitsToFloat(lights[lightID].params0.yzw);
}

uint shadowedLightShadowID(in uint lightID)
{
	return lights[lightID].params2[1u];
}

bool shadowedLightIsVolumetricScatteringEnabled(in uint lightID)
{
	return bitfieldExtract(lights[lightID].params2[0u], 1, 1) != 0u;
}

Range pointLightRadiuses(in uint lightID)
{
	return makeRange(uintBitsToFloat(lights[lightID].params1.xy));
}

Range spotLightRadiuses(in uint lightID)
{
	return makeRange(uintBitsToFloat(lights[lightID].params1.xy));
}

Range spotLightHalfAngles(in uint lightID)
{
	return makeRange(uintBitsToFloat(lights[lightID].params1.zw));
}

uint imageBasedLightBRDFLutMapID(in uint lightID)
{
	return lights[lightID].params1[0u];
}

uint imageBasedLightDiffuseMapID(in uint lightID)
{
	return lights[lightID].params1[1u];
}

uint imageBasedLightSpecularMapID(in uint lightID)
{
	return lights[lightID].params1[2u];
}

float imageBasedLightContribution(in uint lightID)
{
	return uintBitsToFloat(lights[lightID].params1[3u]);
}