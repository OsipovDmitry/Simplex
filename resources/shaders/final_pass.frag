#include<geometry.glsl>
#include<tone_mapping.glsl>

out vec4 o_fragColor0;

void main(void)
{
	const ivec2 fragCoords = ivec2(gl_FragCoord.xy);
	
	vec3 color = geometryBufferFetchFinalColor(fragCoords);
	toneMappingApplyExposure(color);
	toneMappingApplyACES(color);
	toneMappingApplyGammaCorrection(color);

	o_fragColor0 = vec4(color, 1.0f);
}
