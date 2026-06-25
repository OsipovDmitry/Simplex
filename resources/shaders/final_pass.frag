#include<geometry.glsl>

#include<math/gamma_correction.glsl>

out vec4 o_fragColor0;

void main(void)
{
	const ivec2 fragCoords = ivec2(gl_FragCoord.xy);

	o_fragColor0 = vec4(toSRGB(geometryBufferFinalColor(fragCoords)), 1.0f);
}
