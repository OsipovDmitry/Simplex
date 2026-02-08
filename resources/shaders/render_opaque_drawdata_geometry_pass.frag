#extension GL_ARB_bindless_texture : enable

#include<gammacorrection.glsl>
#include<packing.glsl>
#include<mesh.glsl>
#include<material.glsl>
#include<material_map.glsl>

flat in uint v_meshID;
flat in uint v_materialID;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_binormal;
in vec2 v_texCoords;
in vec4 v_color;

out uvec4 o_fragColor0;

void main(void)
{
	if (!gl_FrontFacing && !isMaterialDoubleSided(v_materialID))
		discard;

	const bool hasTexCoords = meshTexCoordsComponentsCount(v_meshID) > 0u;
	
    vec4 baseColor = materialBaseColor(v_materialID);
	baseColor *= v_color;
	
	const uint baseColorMapID = materialBaseColorMapID(v_materialID);
	if (hasTexCoords && (baseColorMapID != 0xFFFFFFFFu))
	{
		const TextureHandle textureHandle = materialMapTextureHandle(baseColorMapID);
		if (!isTextureHandleEmpty(textureHandle))
			baseColor *= toLinearRGB(texture(sampler2D(textureHandle), v_texCoords));
	}
	
	const uint opacityMapID = materialOpacityMapID(v_materialID);
	if (hasTexCoords && (opacityMapID != 0xFFFFFFFFu))
	{
		const TextureHandle textureHandle = materialMapTextureHandle(opacityMapID);
		if (!isTextureHandleEmpty(textureHandle))
			baseColor.a *= texture(sampler2D(textureHandle), v_texCoords).r;
	}

	if (baseColor.a < materialAlphaCutoff(v_materialID))
		discard;
	
	vec3 emission = materialEmission(v_materialID);
	
	const uint emissionMapID = materialEmissionMapID(v_materialID);
	if (hasTexCoords && (emissionMapID != 0xFFFFFFFFu))
	{
		const TextureHandle textureHandle = materialMapTextureHandle(emissionMapID);
		if (!isTextureHandleEmpty(textureHandle))
			emission *= toLinearRGB(texture(sampler2D(textureHandle), v_texCoords).rgb);
	}
	
	const uvec3 ORMSwizzleMask = materialORMSwizzleMask(v_materialID);
	
	float occlusion = 1.0f;
	
	const uint occlusionMapID = materialOcclusionMapID(v_materialID);
    if (hasTexCoords && (occlusionMapID != 0xFFFFFFFFu))
	{
		const TextureHandle textureHandle = materialMapTextureHandle(occlusionMapID);
		if (!isTextureHandleEmpty(textureHandle))
		{
			occlusion *= texture(sampler2D(textureHandle), v_texCoords)[ORMSwizzleMask[0u]];
			occlusion *= materialOcclusionMapStrength(v_materialID);
		}
    }
	
	float roughness = materialRoughness(v_materialID);
	
	const uint roughnessMapID = materialRoughnessMapID(v_materialID);
	if (hasTexCoords && (roughnessMapID != 0xFFFFFFFFu))
	{
		const TextureHandle textureHandle = materialMapTextureHandle(roughnessMapID);
		if (!isTextureHandleEmpty(textureHandle))
			roughness *= texture(sampler2D(textureHandle), v_texCoords)[ORMSwizzleMask[1u]];
	}
	
	float metalness = materialMetalness(v_materialID);
	
	const uint metalnessMapID = materialMetalnessMapID(v_materialID);
	if (hasTexCoords && (metalnessMapID != 0xFFFFFFFFu))
	{
		const TextureHandle textureHandle = materialMapTextureHandle(metalnessMapID);
		if (!isTextureHandleEmpty(textureHandle))
			metalness *= texture(sampler2D(textureHandle), v_texCoords)[ORMSwizzleMask[2u]];
	}
	
	vec3 normal = normalize(v_normal);

	const uint normalMapID = materialNormalMapID(v_materialID);
	if (hasTexCoords && meshTangentFlag(v_meshID) && (normalMapID != 0xFFFFFFFFu))
	{
		const TextureHandle textureHandle = materialMapTextureHandle(normalMapID);
		if (!isTextureHandleEmpty(textureHandle))
		{
			vec3 localNormal = unpackNormal(texture(sampler2D(textureHandle), v_texCoords).xyz);
			localNormal.xy *= materialNormalMapScale(v_materialID);
			localNormal = normalize(localNormal);
			normal = normalize(mat3(normalize(v_tangent), normalize(v_binormal), normal) * localNormal);
		}
	}
	
	if (!gl_FrontFacing)
		normal = -normal;
	
	o_fragColor0 = packPBRData(
		baseColor.rgb,
		emission,
		occlusion,
		roughness,
		metalness,
		1.0f,
		normal,
		isMaterialLighted(v_materialID),
		isMaterialShadowed(v_materialID));
}
