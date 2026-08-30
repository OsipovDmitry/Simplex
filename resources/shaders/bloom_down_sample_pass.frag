#include<hdr.glsl>
#include<render_info.glsl>

in vec2 v_fragCoords;

out vec4 o_fragColor0;

void main(void)
{
	const uint sourceLevel = HDRBloomPassIndex();

    const vec3 a = HDRColor(v_fragCoords, sourceLevel, ivec2(-2, 2));
    const vec3 b = HDRColor(v_fragCoords, sourceLevel, ivec2(0, 2));
    const vec3 c = HDRColor(v_fragCoords, sourceLevel, ivec2(2, 2));

    const vec3 d = HDRColor(v_fragCoords, sourceLevel, ivec2(-2, 0));
    const vec3 e = HDRColor(v_fragCoords, sourceLevel, ivec2(0, 0));
    const vec3 f = HDRColor(v_fragCoords, sourceLevel, ivec2(2, 0));

    const vec3 g = HDRColor(v_fragCoords, sourceLevel, ivec2(-2, -2));
    const vec3 h = HDRColor(v_fragCoords, sourceLevel, ivec2(0, -2));
    const vec3 i = HDRColor(v_fragCoords, sourceLevel, ivec2(2, -2));

    const vec3 j = HDRColor(v_fragCoords, sourceLevel, ivec2(-1, 1));
    const vec3 k = HDRColor(v_fragCoords, sourceLevel, ivec2(1, 1));
    const vec3 l = HDRColor(v_fragCoords, sourceLevel, ivec2(-1, -1));
    const vec3 m = HDRColor(v_fragCoords, sourceLevel, ivec2(1, -1));
	
	o_fragColor0 = vec4(
		0.125f * e +
		0.03125f * (a + c + g + i) + 
		0.0625f * (b + d + f + h) +
		0.125f * (j + k + l + m),
		1.0f);
}
