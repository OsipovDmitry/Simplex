#include<descriptions.glsl>

layout (std430) readonly buffer ssbo_materialsBuffer { MaterialDescription materials[]; };

vec4 materialBaseColor(in uint materialID)
{
	return materials[materialID].baseColor;
}

vec3 materialEmission(in uint materialID)
{
	return materials[materialID].emission.rgb;
}

float materialRoughness(in uint materialID)
{
	return float(bitfieldExtract(materials[materialID].flags0, 0, 8)) / 255.0f;
}

float materialMetalness(in uint materialID)
{
	return float(bitfieldExtract(materials[materialID].flags0, 8, 8)) / 255.0f;
}

float materialAlphaCutoff(in uint materialID)
{
	return float(bitfieldExtract(materials[materialID].flags1, 10, 8)) / 255.0f;
}

uint materialBaseColorMapID(in uint materialID)
{
	return materials[materialID].baseColorMapID;
}

uint materialOpacityMapID(in uint materialID)
{
	return materials[materialID].opacityMapID;
}

uint materialEmissionMapID(in uint materialID)
{
	return materials[materialID].emissionMapID;
}

uint materialOcclusionMapID(in uint materialID)
{
	return materials[materialID].occlusionMapID;
}

uint materialRoughnessMapID(in uint materialID)
{
	return materials[materialID].roughnessMapID;
}

uint materialMetalnessMapID(in uint materialID)
{
	return materials[materialID].metalnessMapID;
}

uint materialNormalMapID(in uint materialID)
{
	return materials[materialID].normalMapID;
}

float materialOcclusionMapStrength(in uint materialID)
{
	return float(bitfieldExtract(materials[materialID].flags0, 16, 8)) / 255.0f;
}

float materialNormalMapScale(in uint materialID)
{
	return float(bitfieldExtract(materials[materialID].flags0, 24, 8)) / 255.0f;
}

uvec3 materialORMSwizzleMask(in uint materialID)
{
	return uvec3(
		bitfieldExtract(materials[materialID].flags1, 0, 2),
		bitfieldExtract(materials[materialID].flags1, 2, 2),
		bitfieldExtract(materials[materialID].flags1, 4, 2));
}

bool isMaterialLighted(in uint materialID)
{
	return bitfieldExtract(materials[materialID].flags1, 6, 1) != 0u;
}

bool isMaterialShadowed(in uint materialID)
{
	return bitfieldExtract(materials[materialID].flags1, 7, 1) != 0u;
}

bool isMaterialShadowCasted(in uint materialID)
{
	return bitfieldExtract(materials[materialID].flags1, 8, 1) != 0u;
}

bool isMaterialDoubleSided(in uint materialID)
{
	return bitfieldExtract(materials[materialID].flags1, 9, 1) != 0u;
}

bool isMaterialTransparent(in uint materialID)
{
	return bitfieldExtract(materials[materialID].flags1, 10, 1) != 0u;
}