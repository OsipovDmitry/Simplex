#include<descriptions.glsl>

layout (std430) buffer ssbo_bloomBuffer { BloomDescription bloom; };

ivec2 bloomTextureSize(in int levelOffset)
{
	return textureSize(sampler2D(bloom.textureHandle), int(bloom.passIndex) + levelOffset);
}

vec3 bloomColor(in vec2 fragCoords, in int levelOffset)
{
	return textureLod(sampler2D(bloom.textureHandle), fragCoords, float(int(bloom.passIndex) + levelOffset)).rgb;
}

float bloomContribution()
{
	return bloom.contribution;
}

float bloomUpSamplePassBlurRadius()
{
	return bloom.upSamplePassBlurRadius;
}