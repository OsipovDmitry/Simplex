#include<map.glsl>
#include<material.glsl>
#include<mesh.glsl>

#include<math/gamma_correction.glsl>

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
	
    const vec3 c1 = mix(vec3(1.0f), baseColor.rgb, baseColor.a);
    const vec3 c2 = mix(baseColor.rgb, vec3(0.0f), baseColor.a);
    
    o_fragColor0 = vec4(mix(c1, c2, baseColor.a), 1.0f);
}