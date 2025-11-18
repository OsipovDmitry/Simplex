#extension GL_ARB_bindless_texture : enable

layout (early_fragment_tests) in;

#include<gammacorrection.glsl>
#include<packing.glsl>
#include<mesh.glsl>
#include<material.glsl>
#include<material_map.glsl>
#include<oit_node.glsl>

layout (r32ui) uniform uimage2DRect u_OITIndicesMap;
layout (std430) buffer ssbo_OITBuffer {
	OITBufferReservedData OITReservedData;
	OITBufferNode OITNodes[];
};

flat in uint v_meshID;
flat in uint v_materialID;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_binormal;
in vec2 v_texCoords;
in vec4 v_color;

void main(void)
{
	bool hasTexCoords = meshTexCoordsComponentsCount(v_meshID) > 0u;
	
    vec4 baseColor = materialBaseColor(v_materialID);
	baseColor *= v_color;
	
	uint baseColorMapID = materialBaseColorMapID(v_materialID);
	if (hasTexCoords && (baseColorMapID != 0xFFFFFFFFu))
		baseColor *= toLinearRGB(texture(sampler2D(materialMap(baseColorMapID)), v_texCoords));
	
	float alpha = baseColor.a;
	if (alpha < materialAlphaCutoff(v_materialID))
		discard;
	
	vec3 emission = materialEmission(v_materialID);
	
	uint emissionMapID = materialEmissionMapID(v_materialID);
	if (hasTexCoords && (emissionMapID != 0xFFFFFFFFu))
		emission *= toLinearRGB(texture(sampler2D(materialMap(emissionMapID)), v_texCoords).rgb);
	
	bool isLighted = isMaterialLighted(v_materialID);
    if (!isLighted)
    {
        emission += baseColor.rgb;
        baseColor.rgb = vec3(0.0f);
    }
	
	uvec3 ORMSwizzleMask = materialORMSwizzleMask(v_materialID);
	
	float occlusion = 1.0f;
	
	uint occlusionMapID = materialOcclusionMapID(v_materialID);
    if (hasTexCoords && (occlusionMapID != 0xFFFFFFFFu))
	{
        occlusion *= texture(sampler2D(materialMap(occlusionMapID)), v_texCoords)[ORMSwizzleMask[0u]];
		occlusion *= materialOcclusionMapStrength(v_materialID);
    }
	
	float roughness = materialRoughness(v_materialID);
	
	uint roughnessMapID = materialRoughnessMapID(v_materialID);
	if (hasTexCoords && (roughnessMapID != 0xFFFFFFFFu))
		roughness *= texture(sampler2D(materialMap(roughnessMapID)), v_texCoords)[ORMSwizzleMask[1u]];
	
	float metalness = materialMetalness(v_materialID);
	
	uint metalnessMapID = materialMetalnessMapID(v_materialID);
	if (hasTexCoords && (metalnessMapID != 0xFFFFFFFFu))
	{
		metalness *= texture(sampler2D(materialMap(metalnessMapID)), v_texCoords)[ORMSwizzleMask[2u]];
	}
	
	vec3 normal = v_normal;

	uint normalMapID = materialNormalMapID(v_materialID);
	if (hasTexCoords && meshTangentFlag(v_meshID) && (normalMapID != 0xFFFFFFFFu))
	{
		vec3 tangent = normalize(v_tangent);
		vec3 binormal = normalize(v_binormal);
		normal = mat3(tangent, binormal, normal) * unpackNormal(texture(sampler2D(materialMap(normalMapID)), v_texCoords).xyz);
		normal.xy *= materialNormalMapScale(v_materialID);
		normal = normalize(normal);
	}
	
    uint OITIndex = atomicAdd(OITReservedData.nodesCount, 1u);
	if (OITIndex < OITReservedData.maxNodesCount)
    {
		uint prevOITIndex = imageAtomicExchange(u_OITIndicesMap, ivec2(gl_FragCoord.xy), OITIndex);
        OITNodes[OITIndex] = packOITBufferNode(
			baseColor.rgb,
			emission,
			occlusion,
			roughness,
			metalness,
			alpha,
			normal,
			isLighted,
			isMaterialShadowed(v_materialID),
			gl_FragCoord.z,
			prevOITIndex);
    }
}
