struct MaterialDescription
{
    vec4 baseColor;
    vec4 emission;
    uint baseColorTextureID;
    uint emissionTextureID;
    uint occlusionTextureID;
    uint roughnessTextureID;
    uint metalnessTextureID;
    uint normalTextureID;
    uint flags0; // 0.. 7 - roughness, 8..15 - metalness, 16..23 - occlusionMapStrength, 24..31 - normalMapScale
    uint flags1;
    //  0.. 1 - occlusion map swizzle
    //  2.. 3 - roughness map swizzle
    //  4.. 5 - metalness map swizzle
    //  6.. 6 - is lighted
    //  7.. 7 - is shadowed
    //  8.. 8 - is transparent
    //  9.. 9 - is double sided
    // 10..17 - alpha cutoff
    // 18..31 - free (14 bits)
};

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
	return materials[materialID].baseColorTextureID;
}

uint materialEmissionMapID(in uint materialID)
{
	return materials[materialID].emissionTextureID;
}

uint materialOcclusionMapID(in uint materialID)
{
	return materials[materialID].occlusionTextureID;
}

uint materialRoughnessMapID(in uint materialID)
{
	return materials[materialID].roughnessTextureID;
}

uint materialMetalnessMapID(in uint materialID)
{
	return materials[materialID].metalnessTextureID;
}

uint materialNormalMapID(in uint materialID)
{
	return materials[materialID].normalTextureID;
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

bool isMaterialDoubleSided(in uint materialID)
{
	return bitfieldExtract(materials[materialID].flags1, 9, 1) != 0u;
}

bool isMaterialTransparent(in uint materialID)
{
	return bitfieldExtract(materials[materialID].flags1, 8, 1) != 0u;
}