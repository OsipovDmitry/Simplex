#extension GL_ARB_bindless_texture : enable

layout (early_fragment_tests) in;

#include<descriptions.glsl>
#include<gammacorrection.glsl>
#include<oit_node.glsl>

layout (std430) readonly buffer ssbo_meshesBuffer { MeshDescription meshes[]; };
layout (std430) readonly buffer ssbo_materialMapsBuffer { uvec2 materialMaps[]; };
layout (std430) readonly buffer ssbo_materialsBuffer { MaterialDescription materials[]; };

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
	MeshDescription meshDescription = meshes[v_meshID];
	MaterialDescription materialDescription = materials[v_materialID];
	
	bool hasTexCoords = meshTexCoordsComponentsCount(meshDescription) > 0u;
	bool hasTangent = meshTangentFlag(meshDescription);
	bool isLighted = isMaterialLighted(materialDescription);
	bool isShadowed = isMaterialShadowed(materialDescription);
	
    vec4 baseColor = materialDescription.baseColor;
	if (meshColorComponentsCount(meshDescription) > 0u)
	{
		baseColor *= v_color;
	}
	if (hasTexCoords && (materialDescription.baseColorTextureID != 0xFFFFFFFFu))
	{
		baseColor *= toLinearRGB(texture(sampler2D(materialMaps[materialDescription.baseColorTextureID]), v_texCoords));
	}
	
	float alpha = baseColor.a;
	if (alpha < materialAlphaCutoff(materialDescription))
		discard;
	
	vec3 emission = materialDescription.emission.rgb;
	if (hasTexCoords && (materialDescription.emissionTextureID != 0xFFFFFFFFu))
	{
		emission *= toLinearRGB(texture(sampler2D(materialMaps[materialDescription.emissionTextureID]), v_texCoords)).rgb;
    }
    if (!isLighted)
    {
        emission += baseColor.rgb;
        baseColor.rgb = vec3(0.0f);
    }
	
	float occlusion = 1.0f;
    if (hasTexCoords && (materialDescription.occlusionTextureID != 0xFFFFFFFFu))
	{
        occlusion *= texture(sampler2D(materialMaps[materialDescription.occlusionTextureID]), v_texCoords)[materialOcclusionMapSwizzle(materialDescription)];
		occlusion *= materialOcclusionMapStrength(materialDescription);
    }
	
	float roughness = materialRoughness(materialDescription);
	if (hasTexCoords && (materialDescription.roughnessTextureID != 0xFFFFFFFFu))
	{
		roughness *= texture(sampler2D(materialMaps[materialDescription.roughnessTextureID]), v_texCoords)[materialRoughnessMapSwizzle(materialDescription)];
	}
	
	float metalness = materialMetalness(materialDescription);
	if (hasTexCoords && (materialDescription.metalnessTextureID != 0xFFFFFFFFu))
	{
		metalness *= texture(sampler2D(materialMaps[materialDescription.metalnessTextureID]), v_texCoords)[materialMetalnessMapSwizzle(materialDescription)];
	}
	
	vec3 normal = v_normal;
	if (hasTangent && hasTexCoords && (materialDescription.normalTextureID != 0xFFFFFFFFu))
	{
		vec3 tangent = normalize(v_tangent);
		vec3 binormal = normalize(v_binormal);
		normal = mat3(tangent, binormal, normal) * unpackNormal(texture(sampler2D(materialMaps[materialDescription.normalTextureID]), v_texCoords).xyz);
		normal.xz *= materialNormalMapScale(materialDescription);
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
			isShadowed,
			gl_FragCoord.z,
			prevOITIndex);
    }
}
