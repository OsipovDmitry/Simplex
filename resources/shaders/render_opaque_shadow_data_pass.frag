#include<map.glsl>
#include<material.glsl>
#include<mesh.glsl>
#include<shadow_maps.glsl>

#include<math/gamma_correction.glsl>
#include<math/shadow_utils.glsl>

flat in uint g_meshID;
flat in uint g_materialID;
in float g_linearNormalizedDepth;
in vec2 g_texCoords;

out vec4 o_fragColor0;

void main(void)
{
	if (!gl_FrontFacing && !isMaterialDoubleSided(g_materialID))
		discard;
		
	const bool hasTexCoords = meshHasTexCoords(g_meshID);
	
    vec4 baseColor = materialBaseColor(g_materialID);
	
	const uint baseColorMapID = materialBaseColorMapID(g_materialID);
	if (hasTexCoords && (baseColorMapID != 0xFFFFFFFFu))
	{
		const TextureHandle textureHandle = mapTextureHandle(baseColorMapID);
		if (!isTextureHandleEmpty(textureHandle))
			baseColor *= toLinearRGB(texture(sampler2D(textureHandle), g_texCoords));
	}
	
	const uint opacityMapID = materialOpacityMapID(g_materialID);
	if (hasTexCoords && (opacityMapID != 0xFFFFFFFFu))
	{
		const TextureHandle textureHandle = mapTextureHandle(opacityMapID);
		if (!isTextureHandleEmpty(textureHandle))
			baseColor.a *= texture(sampler2D(textureHandle), g_texCoords).r;
	}

	if (baseColor.a < materialAlphaCutoff(g_materialID))
		discard;
	
	#if (SHADOW_FILTER == DISCRETE_SHADOW_FILTER)
		o_fragColor0 = calculateDiscreteMoments(g_linearNormalizedDepth);
	#elif (SHADOW_FILTER == VSM_SHADOW_FILTER)
		o_fragColor0 = calculateVSMMoments(g_linearNormalizedDepth);
	#elif (SHADOW_FILTER == EVSM_SHADOW_FILTER)
		o_fragColor0 = calculateEVSMMoments(
			g_linearNormalizedDepth,
			shadowMapsPositiveExponent(),
			shadowMapsNegativeExponent());
	#elif ((SHADOW_FILTER == HAMBURGER_MSM_SHADOW_FILTER) || (SHADOW_FILTER == HAUSDORFF_MSM_SHADOW_FILTER))
		o_fragColor0 = calculateMSMMoments(g_linearNormalizedDepth);
	#else
		error!!!
	#endif
}