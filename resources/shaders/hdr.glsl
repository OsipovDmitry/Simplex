#include<descriptions.glsl>

layout (std430) buffer ssbo_HDRBuffer { HDRDescription HDRBuffer; };

vec3 HDRFetchColor(in ivec2 fragCoords)
{
	return texelFetch(sampler2D(HDRBuffer.textureHandle), fragCoords, 0).rgb;
}

vec3 HDRColor(in vec2 fragCoords, in uint level, in ivec2 fragCoordsOffset)
{
	return textureLodOffset(sampler2D(HDRBuffer.textureHandle), fragCoords, float(level), fragCoordsOffset).rgb;
}

float HDRBloomContribution()
{
	return HDRBuffer.bloomContribution;
}

uint HDRBloomPassIndex()
{
	return HDRBuffer.bloomPassIndex;
}
