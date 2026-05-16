#include<descriptions.glsl>

layout (std430) readonly buffer ssbo_shadowMapsBuffer { ShadowMapsDescription shadowMaps; };

TextureHandle shadowMapsShadowDepthTextureHandle()
{
	return shadowMaps.shadowDepthMapTextureHandle;
}

TextureHandle shadowMapsShadowVarianceTextureHandle()
{
	return shadowMaps.shadowVarianceTextureHandle;
}

TextureHandle shadowMapsShadowColorTextureHandle()
{
	return shadowMaps.shadowColorTextureHandle;
}