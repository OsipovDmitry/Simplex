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

vec4 Get4MomentsOptimized(in float FragmentDepth)
{
	const float Square = FragmentDepth * FragmentDepth;
	const vec4 Moments = vec4(FragmentDepth, Square, Square*FragmentDepth, Square*Square);
	
	return vec4(0.035955884801f, vec3(0.0f)) + mat4x4(
		-2.07224649f, 13.7948857237f, 0.105877704f, 9.7924062118f,
		32.23703778f, -59.4683975703f, -1.9077466311f, -33.7652110555f,
		-68.571074599f, 82.0359750338f, 9.3496555107f, 47.9456096605f,
		39.3703274134f, -35.364903257f, -6.6543490743f, -23.9728048165f) * Moments;
}

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
	
	//o_fragColor0 = Get4MomentsOptimized(g_linearNormalizedDepth);
	
    const float dx = dFdx(g_linearNormalizedDepth);
    const float dy = dFdy(g_linearNormalizedDepth);
	const float bias = 0.25 * (dx * dx + dy * dy);
	const float d = g_linearNormalizedDepth;
	const float d2 = d * d;
	const float d3 = d2 * d;
	const float d4 = d3 * d;
	o_fragColor0 = vec4(
		d, 
        d2 + bias, 
        d3 + 3.0 * d * bias, 
        d4 + 6.0 * d2 * bias + 3.0 * bias * bias);
}