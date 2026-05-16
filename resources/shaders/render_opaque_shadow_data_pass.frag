#extension GL_ARB_bindless_texture : enable

#include<map.glsl>
#include<material.glsl>
#include<mesh.glsl>

#include<math/gamma_correction.glsl>

flat in uint g_meshID;
flat in uint g_materialID;
in float g_linearNormalizedDepth;
in vec2 g_texCoords;
in vec4 g_color;

out vec4 o_fragColor0;

void main(void)
{
	if (!gl_FrontFacing && !isMaterialDoubleSided(g_materialID))
		discard;

	const bool hasTexCoords = meshTexCoordsComponentsCount(g_meshID) > 0u;
	
    vec4 baseColor = materialBaseColor(g_materialID);
	baseColor *= g_color;
	
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
	
    const float dx = dFdx(g_linearNormalizedDepth);
    const float dy = dFdy(g_linearNormalizedDepth);
    o_fragColor0 = vec4(
		g_linearNormalizedDepth,
		g_linearNormalizedDepth * g_linearNormalizedDepth + 0.25f * (dx * dx + dy * dy),
		0.0f,
		0.0f);
}