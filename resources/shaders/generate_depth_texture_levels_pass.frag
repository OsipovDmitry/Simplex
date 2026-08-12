#include<geometry.glsl>

void main(void)
{
	const ivec2 sourceFragCoords = 2 * ivec2(gl_FragCoord.xy);
	const uint passIndex = geometryBufferGenerateDepthTextureLevelsPassIndex();
	
	const float d0 = geometryBufferDepth(sourceFragCoords + ivec2(0, 0), passIndex);
	const float d1 = geometryBufferDepth(sourceFragCoords + ivec2(1, 0), passIndex);
	const float d2 = geometryBufferDepth(sourceFragCoords + ivec2(0, 1), passIndex);
	const float d3 = geometryBufferDepth(sourceFragCoords + ivec2(1, 1), passIndex);
	
	const float maxDepth = max(max(d0, d1), max(d2, d3));
	
	gl_FragDepth = maxDepth;
}
