#include<geometry.glsl>

void main(void)
{
	gl_FragDepth = geometryBufferCalculateDepthTextureLevel(ivec2(gl_FragCoord.xy));
}
