#include<geometry.glsl>

#include<math/gamma_correction.glsl>

// tmp
#include<bloom.glsl>

out vec4 o_fragColor0;

void main(void)
{
	const ivec2 fragCoords = ivec2(gl_FragCoord.xy);
	
	vec3 color = geometryBufferFetchFinalColor(fragCoords);
	//color /= (vec3(1.0f) + color);

	o_fragColor0 = vec4(toSRGB(color), 1.0f);
	
	//tmp
	//const int COUNT = textureQueryLevels(sampler2D(bloom.textureHandle));
	//int W = 0;
	
	//for (int level = 0; level < COUNT; ++level)
	//{
	//	const ivec2 levelSize = textureSize(sampler2D(bloom.textureHandle), level);
	//	const ivec2 tc = fragCoords - ivec2(W, 0);
	//	if (all(lessThanEqual(ivec2(0, 0), tc)) && all(lessThan(tc, levelSize)))
	//	{
	//		o_fragColor0 = vec4(texelFetch(sampler2D(bloom.textureHandle), tc, level).rgb, 1.0f);
	//	}	
	//	W += levelSize.x;
	//}
}
