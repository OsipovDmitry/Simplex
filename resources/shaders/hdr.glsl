#include<descriptions.glsl>

layout (std430) buffer ssbo_HDRBuffer { HDRDescription HDRBuffer; };

vec3 HDRBufferFetchFinalColor(in ivec2 fragCoords)
{
	return texelFetch(sampler2DRect(HDRBuffer.textureHandle), fragCoords).rgb;
}

vec3 HDRBufferFinalColor(in vec2 fragCoords)
{
	return texture(sampler2DRect(HDRBuffer.textureHandle), fragCoords).rgb;
}