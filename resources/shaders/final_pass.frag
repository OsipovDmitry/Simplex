#include<geometry.glsl>

#include<math/gamma_correction.glsl>

// tmp
#include<shadow_maps.glsl>

out vec4 o_fragColor0;

void main(void)
{
	const ivec2 fragCoords = ivec2(gl_FragCoord.xy);

	o_fragColor0 = vec4(toSRGB(geometryBufferFinalColor(fragCoords)), 1.0f);
	
	//const int SIZE = 1024;
	//if (all(lessThan(fragCoords, ivec2(SIZE))))
	//{
	//	const ivec2 tc = ivec2(vec2(fragCoords) / vec2(float(SIZE)) * vec2(float(shadowMapsAtlasSize()))); 
	//	float c = texelFetch(sampler2DArray(shadowMaps.depthTextureHandle), ivec3(tc, 0), 0).r;
	//	
	//	o_fragColor0 = vec4(c, c, c, 1.0f);
	//}
}
