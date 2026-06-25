#include<render_info.glsl>
#include<shadow_maps.glsl>

flat in ivec2 g_minMapPoint;
flat in ivec2 g_maxMapPoint;

out vec4 o_fragColor0;
out vec4 o_fragColor1;

vec4 fetchMomentsTexel(in ivec2 coords)
{
#if defined(HORIZONTAL_PASS)
	return shadowMapsFetchMomentsTexel(ivec3(clamp(coords, g_minMapPoint, g_maxMapPoint), gl_Layer));
#elif defined(VERTICAL_PASS)
	return renderInfoFetchShadowMapBluredMomentsTexel(ivec3(clamp(coords, g_minMapPoint, g_maxMapPoint), gl_Layer));
#else
	error!!!
#endif
}

vec3 fetchColorTexel(in ivec2 coords)
{
#if defined(HORIZONTAL_PASS)
	return shadowMapsFetchColorTexel(ivec3(clamp(coords, g_minMapPoint, g_maxMapPoint), gl_Layer));
#elif defined(VERTICAL_PASS)
	return renderInfoFetchShadowMapBluredColorTexel(ivec3(clamp(coords, g_minMapPoint, g_maxMapPoint), gl_Layer));
#else
	error!!!
#endif
}

void main(void)
{
	const ivec2 fragCoords = ivec2(gl_FragCoord.xy);
	const int radius = int(renderInfoShadowBlurRadius());
	
#if defined(HORIZONTAL_PASS)
	const ivec2 direction = ivec2(1, 0);
#elif defined(VERTICAL_PASS)
	const ivec2 direction = ivec2(0, 1);
#else
	error!!!
#endif
	
	vec4 variance = fetchMomentsTexel(fragCoords);
	vec3 color = fetchColorTexel(fragCoords);
	
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
		
		variance += smpl * (fetchMomentsTexel(negTexCoords) + fetchMomentsTexel(posTexCoords));
		color += smpl * (fetchColorTexel(negTexCoords) + fetchColorTexel(posTexCoords));
	}
	
	o_fragColor0 = variance;
	o_fragColor1 = vec4(color, 1.0f);
}