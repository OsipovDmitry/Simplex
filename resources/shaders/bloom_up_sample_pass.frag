#include<bloom.glsl>
#include<tone_mapping.glsl>

in vec2 v_fragCoords;

out vec4 o_fragColor0;

vec3 sourceTextureColor(in vec2 fragCoords)
{
#ifdef LAST_PASS
	return bloomColor(fragCoords, 0);
#else
	return bloomColor(fragCoords, +1);
#endif	
}

void main(void)
{
#ifdef LAST_PASS
	const vec2 sourceTextureSize = vec2(bloomTextureSize(0));
#else
	const vec2 sourceTextureSize = vec2(bloomTextureSize(+1));
#endif
	
	const vec2 fragCoords = v_fragCoords;
	const float x = 1.0f / sourceTextureSize.x;
	const float y = 1.0f / sourceTextureSize.y;
	
	const float radius = bloomUpSamplePassBlurRadius();

    const vec3 a = sourceTextureColor(fragCoords + radius * vec2(  -x, +y));
    const vec3 b = sourceTextureColor(fragCoords + radius * vec2(0.0f, +y));
    const vec3 c = sourceTextureColor(fragCoords + radius * vec2(  +x, +y));

    const vec3 d = sourceTextureColor(fragCoords + radius * vec2(  -x, 0.0f));
    const vec3 e = sourceTextureColor(fragCoords + radius * vec2(0.0f, 0.0f));
    const vec3 f = sourceTextureColor(fragCoords + radius * vec2(  +x, 0.0f));

    const vec3 g = sourceTextureColor(fragCoords + radius * vec2(  -x, -y));
    const vec3 h = sourceTextureColor(fragCoords + radius * vec2(0.0f, -y));
    const vec3 i = sourceTextureColor(fragCoords + radius * vec2(  +x, -y));
	
	vec3 result =
		0.25f * e +
		0.0625f * (a + c + g + i) + 
		0.125f * (b + d + f + h);
	
#ifdef LAST_PASS
	result *= bloomContribution();
	toneMappingApplyInvertedExposure(result);
#endif

	o_fragColor0 = vec4(result, 0.0f);
}
