#include<descriptions.glsl>

#include<math/shadow_utils.glsl>

#define HAMBURGER_MOMENTS_SHADOW_MAPS_ALGORITHM 0
#define HAUSFORFF_MOMENTS_SHADOW_MAPS_ALGORITHM 1

#ifndef MOMENTS_SHADOW_MAPS_ALGORITHM
	#define MOMENTS_SHADOW_MAPS_ALGORITHM HAUSFORFF_MOMENTS_SHADOW_MAPS_ALGORITHM
#endif

layout (std430) readonly buffer ssbo_shadowMapsBuffer { ShadowMapsDescription shadowMaps; };

vec4 shadowMapsFetchMomentsTexel(in ivec3 coords)
{
	return texelFetch(sampler2DArray(shadowMaps.shadowVarianceTextureHandle), coords, 0);
}

vec3 shadowMapsFetchColorTexel(in ivec3 coords)
{
	return texelFetch(sampler2DArray(shadowMaps.shadowColorTextureHandle), coords, 0).rgb;
}

vec3 shadowMapsProcessDiscreteShadow(
	in vec3 NDC_ZO,
	in uvec3 mapCoords,
	in uint mapSize,
	in bool isTexelTransparent)
{
	const ivec3 texelPosSMS = ivec3(NDC_ZO.xy * float(mapSize - 1u) + vec2(0.5f), 0.0f) + ivec3(mapCoords);	
	const float refDepth = texelFetch(sampler2DArray(shadowMaps.shadowDepthTextureHandle), texelPosSMS, 0).r;
	
	vec3 result = vec3(step(NDC_ZO[2u], refDepth));
	if (!isTexelTransparent)
	{
		result *= texelFetch(sampler2DArray(shadowMaps.shadowColorTextureHandle), texelPosSMS, 0).rgb;
	}
	
	return result;
}

vec3 shadowMapsProccessVarianceShadow(
	in vec2 NDC_XY_ZO,
	in float linearNormalizedDepth,
	in uvec3 mapCoords,
	in uint mapSize,
	in float lightBleedingAmount,
	in bool isTexelTransparent)
{
	const sampler2DArray momentsTexture = sampler2DArray(shadowMaps.shadowVarianceTextureHandle);

	const vec2 texelPosSMS = (NDC_XY_ZO * float(mapSize - 1u) + vec2(0.5f)) + vec2(mapCoords.xy);
	const vec3 texCoords = vec3(texelPosSMS.xy / textureSize(momentsTexture, 0).xy, float(mapCoords[2u]));
	
	const vec2 moments = texture(momentsTexture, texCoords).rg;
	
	const float variance = max(moments.y - moments.x * moments.x, EPS);
	const float d = linearNormalizedDepth - moments.x;
	const float p_max = smoothstep(lightBleedingAmount, 1.0f, variance / (variance + d * d));
	
	const float p = step(linearNormalizedDepth, moments.x);
	vec3 result = vec3(max(p, p_max));
	
	if (!isTexelTransparent)
	{
		result *= texture(sampler2DArray(shadowMaps.shadowColorTextureHandle), texCoords).rgb;
	}
	
	return result;
}

vec3 shadowMapsProccessMomentsShadow(
	in vec2 NDC_XY_ZO,
	in float linearNormalizedDepth,
	in uvec3 mapCoords,
	in uint mapSize,
	in float depthBias,
	in float momentsBias,
	in bool isTexelTransparent)
{
	const sampler2DArray momentsTexture = sampler2DArray(shadowMaps.shadowVarianceTextureHandle);

	const vec2 texelPosSMS = (NDC_XY_ZO * float(mapSize - 1u) + vec2(0.5f)) + vec2(mapCoords.xy);
	const vec3 texCoords = vec3(texelPosSMS.xy / textureSize(momentsTexture, 0).xy, float(mapCoords[2u]));
	
	const vec4 moments = texture(momentsTexture, texCoords);
	
	#if (MOMENTS_SHADOW_MAPS_ALGORITHM == HAMBURGER_MOMENTS_SHADOW_MAPS_ALGORITHM)
		vec3 result = vec3(Hamburger4MSMShadowIntensity(moments, linearNormalizedDepth, depthBias, momentsBias));
	#elif (MOMENTS_SHADOW_MAPS_ALGORITHM == HAUSFORFF_MOMENTS_SHADOW_MAPS_ALGORITHM)
		vec3 result = vec3(Hausdorff4MSMShadowIntensity(moments, linearNormalizedDepth, depthBias, momentsBias));
	#else
		error!!!
	#endif
	
	if (!isTexelTransparent)
	{
		result *= texture(sampler2DArray(shadowMaps.shadowColorTextureHandle), texCoords).rgb;
	}
	
	return result;
}

TextureHandle shadowMapsShadowDepthTextureHandle()
{
	return shadowMaps.shadowDepthTextureHandle;
}

TextureHandle shadowMapsShadowVarianceTextureHandle()
{
	return shadowMaps.shadowVarianceTextureHandle;
}

TextureHandle shadowMapsShadowColorTextureHandle()
{
	return shadowMaps.shadowColorTextureHandle;
}