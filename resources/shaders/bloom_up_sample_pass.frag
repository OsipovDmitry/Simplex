#include<hdr.glsl>
#include<tone_mapping.glsl>

in vec2 v_fragCoords;

out vec4 o_fragColor0;


void main(void)
{
	const uint sourceLevel = HDRBloomPassIndex();

    const vec3 a = HDRColor(v_fragCoords, sourceLevel, ivec2(-1, 1));
    const vec3 b = HDRColor(v_fragCoords, sourceLevel, ivec2(0, 1));
    const vec3 c = HDRColor(v_fragCoords, sourceLevel, ivec2(1, 1));

    const vec3 d = HDRColor(v_fragCoords, sourceLevel, ivec2(-1, 0));
    const vec3 e = HDRColor(v_fragCoords, sourceLevel, ivec2(0, 0));
    const vec3 f = HDRColor(v_fragCoords, sourceLevel, ivec2(1, 0));

    const vec3 g = HDRColor(v_fragCoords, sourceLevel, ivec2(-1, -1));
    const vec3 h = HDRColor(v_fragCoords, sourceLevel, ivec2(0, -1));
    const vec3 i = HDRColor(v_fragCoords, sourceLevel, ivec2(1, -1));
	
	vec3 result =
		0.25f * e +
		0.0625f * (a + c + g + i) + 
		0.125f * (b + d + f + h);
	
	if (sourceLevel == 1u)
	{
		result *= HDRBloomContribution();
		toneMappingApplyInvertedExposure(result);
	}

	o_fragColor0 = vec4(result, 0.0f);
}
