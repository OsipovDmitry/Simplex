#extension GL_ARB_bindless_texture : enable

layout (early_fragment_tests) in;

#include<gammacorrection.glsl>
#include<geometry.glsl>
#include<packing.glsl>
#include<mesh.glsl>
#include<material.glsl>
#include<material_map.glsl>
#include<scene_info.glsl>

flat in uint v_meshID;
flat in uint v_materialID;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_binormal;
in vec2 v_texCoords;
in vec4 v_color;

void main(void)
{
	if (!gl_FrontFacing && !isMaterialDoubleSided(v_materialID))
		discard;

	const bool hasTexCoords = meshTexCoordsComponentsCount(v_meshID) > 0u;
	
    vec4 baseColor = materialBaseColor(v_materialID);
	baseColor *= v_color;
	
	const uint baseColorMapID = materialBaseColorMapID(v_materialID);
	if (hasTexCoords && (baseColorMapID != 0xFFFFFFFFu))
		baseColor *= toLinearRGB(texture(sampler2D(materialMapTextureHandle(baseColorMapID)), v_texCoords));
	
	vec3 emission = materialEmission(v_materialID);
	
	const uint emissionMapID = materialEmissionMapID(v_materialID);
	if (hasTexCoords && (emissionMapID != 0xFFFFFFFFu))
		emission *= toLinearRGB(texture(sampler2D(materialMapTextureHandle(emissionMapID)), v_texCoords).rgb);
	
	const uvec3 ORMSwizzleMask = materialORMSwizzleMask(v_materialID);
	
	float occlusion = 1.0f;
	
	const uint occlusionMapID = materialOcclusionMapID(v_materialID);
    if (hasTexCoords && (occlusionMapID != 0xFFFFFFFFu))
	{
        occlusion *= texture(sampler2D(materialMapTextureHandle(occlusionMapID)), v_texCoords)[ORMSwizzleMask[0u]];
		occlusion *= materialOcclusionMapStrength(v_materialID);
    }
	
	float roughness = materialRoughness(v_materialID);
	
	const uint roughnessMapID = materialRoughnessMapID(v_materialID);
	if (hasTexCoords && (roughnessMapID != 0xFFFFFFFFu))
		roughness *= texture(sampler2D(materialMapTextureHandle(roughnessMapID)), v_texCoords)[ORMSwizzleMask[1u]];
	
	float metalness = materialMetalness(v_materialID);
	
	const uint metalnessMapID = materialMetalnessMapID(v_materialID);
	if (hasTexCoords && (metalnessMapID != 0xFFFFFFFFu))
	{
		metalness *= texture(sampler2D(materialMapTextureHandle(metalnessMapID)), v_texCoords)[ORMSwizzleMask[2u]];
	}
	
	vec3 normal = normalize(v_normal);

	const uint normalMapID = materialNormalMapID(v_materialID);
	if (hasTexCoords && meshTangentFlag(v_meshID) && (normalMapID != 0xFFFFFFFFu))
	{
		vec3 localNormal = unpackNormal(texture(sampler2D(materialMapTextureHandle(normalMapID)), v_texCoords).xyz);
		localNormal.xy *= materialNormalMapScale(v_materialID);
		localNormal = normalize(localNormal);
		normal = normalize(mat3(normalize(v_tangent), normalize(v_binormal), normal) * localNormal);
	}
	
    const uint OITNodeID = geometryBufferGenerateOITNodeID();
	if (OITNodeID < geometryBufferOITNodesMaxCount())
    {
		const uint nextOITNodeID = geometryBufferSetFirstOITNodeID(ivec2(gl_FragCoord.xy), OITNodeID);
		geometryBufferInitializeOITNode(
			OITNodeID,
			packPBRData(
				baseColor.rgb,
				emission,
				occlusion,
				roughness,
				metalness,
				baseColor.a,
				normal,
				isMaterialLighted(v_materialID),
				isMaterialShadowed(v_materialID)),
			gl_FragCoord.z,
			nextOITNodeID);
    }
}
