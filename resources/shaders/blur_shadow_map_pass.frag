#extension GL_ARB_bindless_texture : enable

#include<render_info.glsl>
#include<shadow_maps.glsl>

flat in ivec2 g_minMapPoint;
flat in ivec2 g_maxMapPoint;

out vec4 o_fragColor0;
out vec4 o_fragColor1;

vec4 fetchSourceMapTexel(in sampler2DArray sourceTexture, in ivec2 coords)
{
	const ivec3 clampedCoords = ivec3(clamp(coords, g_minMapPoint, g_maxMapPoint), gl_Layer);
	return texelFetch(sourceTexture, clampedCoords, 0);
}

void main(void)
{
	const ivec2 fragCoords = ivec2(gl_FragCoord.xy);
	const int radius = int(renderInfoShadowBlurRadius());
	
#if defined(HORIZONTAL_PASS)
	const sampler2DArray varianceTexture = sampler2DArray(shadowMapsShadowVarianceTextureHandle());
	const sampler2DArray colorTexture = sampler2DArray(shadowMapsShadowColorTextureHandle());
	const ivec2 direction = ivec2(1, 0);
#elif defined(VERTICAL_PASS)
	const sampler2DArray varianceTexture = sampler2DArray(renderInfoShadowVarianceBluredTextureHandle());
	const sampler2DArray colorTexture = sampler2DArray(renderInfoShadowColorBluredTextureHandle());
	const ivec2 direction = ivec2(0, 1);
#else
	error!!!
#endif
	
	vec4 variance = fetchSourceMapTexel(varianceTexture, fragCoords);
	vec4 color = fetchSourceMapTexel(colorTexture, fragCoords);
	
	if (radius > 0)
	{
		const float smpl0 = renderInfoShadowBlurKernelSample(0u);
		variance *= smpl0;
		color *= smpl0;
	}
	
	for (int r = 1; r < radius; ++r)
	{
		const float smpl = renderInfoShadowBlurKernelSample(uint(r));
		
		const ivec2 negTexCoords = fragCoords - direction * r;
		const ivec2 posTexCoords = fragCoords + direction * r;
		
		variance += smpl * (fetchSourceMapTexel(varianceTexture, negTexCoords) + fetchSourceMapTexel(varianceTexture, posTexCoords));
		color += smpl * (fetchSourceMapTexel(colorTexture, negTexCoords) + fetchSourceMapTexel(colorTexture, posTexCoords));
	}
	
	o_fragColor0 = variance;
	o_fragColor1 = color;
}