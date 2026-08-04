#include<bloom.glsl>
#include<hdr.glsl>
#include<render_info.glsl>

in vec2 v_fragCoords;

out vec4 o_fragColor0;

vec3 sourceTextureColor(in vec2 fragCoords)
{
#ifdef FIRST_PASS
	return HDRBufferFinalColor(fragCoords);
#else
	return bloomColor(fragCoords, -1);
#endif
}

void main(void)
{
#ifdef FIRST_PASS
	const vec2 fragCoords = v_fragCoords * vec2(renderInfoViewportSize());
	const float x = 1.0f;
	const float y = 1.0f;
#else
	const vec2 fragCoords = v_fragCoords;
	const vec2 sourceTextureSize = vec2(bloomTextureSize(-1));
	const float x = 1.0f / sourceTextureSize.x;
	const float y = 1.0f / sourceTextureSize.y;
#endif

    const vec3 a = sourceTextureColor(fragCoords + vec2(-2.0f * x, +2.0f * y));
    const vec3 b = sourceTextureColor(fragCoords + vec2(     0.0f, +2.0f * y));
    const vec3 c = sourceTextureColor(fragCoords + vec2(+2.0f * x, +2.0f * y));

    const vec3 d = sourceTextureColor(fragCoords + vec2(-2.0f * x, 0.0f));
    const vec3 e = sourceTextureColor(fragCoords + vec2(     0.0f, 0.0f));
    const vec3 f = sourceTextureColor(fragCoords + vec2(+2.0f * x, 0.0f));

    const vec3 g = sourceTextureColor(fragCoords + vec2(-2.0f * x, -2.0f * y));
    const vec3 h = sourceTextureColor(fragCoords + vec2(     0.0f, -2.0f * y));
    const vec3 i = sourceTextureColor(fragCoords + vec2(+2.0f * x, -2.0f * y));

    const vec3 j = sourceTextureColor(fragCoords + vec2(-x, +y));
    const vec3 k = sourceTextureColor(fragCoords + vec2(+x, +y));
    const vec3 l = sourceTextureColor(fragCoords + vec2(-x, -y));
    const vec3 m = sourceTextureColor(fragCoords + vec2(+x, -y));
	
	o_fragColor0 = vec4(
		0.125f * e +
		0.03125f * (a + c + g + i) + 
		0.0625f * (b + d + f + h) +
		0.125f * (j + k + l + m),
		1.0f);
}
