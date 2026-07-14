#include<descriptions.glsl>

#include<math/shadow_utils.glsl>

layout (std430) readonly buffer ssbo_shadowMapsBuffer { ShadowMapsDescription shadowMaps; };

vec4 shadowMapsFetchMomentsTexel(in ivec3 coords)
{
	return texelFetch(sampler2DArray(shadowMaps.momentsTextureHandle), coords, 0);
}

vec3 shadowMapsFetchColorTexel(in ivec3 coords)
{
	return texelFetch(sampler2DArray(shadowMaps.colorTextureHandle), coords, 0).rgb;
}

vec4 shadowMapsFetchBluredMomentsTexel(in ivec3 coords)
{
	return texelFetch(sampler2DArray(shadowMaps.momentsBluredTextureHandle), coords, 0);
}

vec3 shadowMapsFetchBluredColorTexel(in ivec3 coords)
{
	return texelFetch(sampler2DArray(shadowMaps.colorBluredTextureHandle), coords, 0).rgb;
}

uint shadowMapsAtlasSize()
{
	return shadowMaps.atlasSize;
}

float shadowMapsLightBleedingAmount()
{
	return shadowMaps.lightBleedingAmount;
}

float shadowMapsPositiveExponent()
{
	return shadowMaps.positiveExponent;
}

float shadowMapsNegativeExponent()
{
	return shadowMaps.negativeExponent;
}

float shadowMapsMomentsBias()
{
	return shadowMaps.momentsBias;
}

float shadowMapsDepthBiasFactor()
{
	return shadowMaps.depthBiasFactor;
}

float shadowMapsCascadesBlendDistanceFactor()
{
	return shadowMaps.cascadesBlendDistanceFactor;
}

float shadowMapsCascadesDistancePower()
{
	return shadowMaps.cascadesDistancePower;
}

float shadowMapsBlurKernelSample(in uint radius)
{
	return shadowMaps.blurKernel[radius];
}

uint shadowMapsBlurRadius()
{
	return shadowMaps.blurRadius;
}

vec3 shadowMapsProcessDiscreteShadow(
	in vec2 NDC_XY_ZO,
	in float linearNormalizedDepth,
	in uvec3 mapCoords,
	in uint mapSize,
	in bool isTexelTransparent)
{
	const sampler2DArray momentsTexture = sampler2DArray(shadowMaps.momentsTextureHandle);

	const ivec2 texelPosSMS = ivec2(NDC_XY_ZO * float(mapSize - 1u) + vec2(0.5f)) + ivec2(mapCoords.xy);
	const ivec3 texCoords = ivec3(texelPosSMS, int(mapCoords[2u]));
	
	const float refDepth = texelFetch(momentsTexture, texCoords, 0).r;
	
	vec3 result = vec3(step(linearNormalizedDepth, refDepth));
	if (!isTexelTransparent)
	{
		result *= texelFetch(sampler2DArray(shadowMaps.colorTextureHandle), texCoords, 0).rgb;
	}
	
	return result;
}

vec3 shadowMapsProccessVSMShadow(
	in vec2 NDC_XY_ZO,
	in float linearNormalizedDepth,
	in uvec3 mapCoords,
	in uint mapSize,
	in float lightBleedingAmount,
	in bool isTexelTransparent)
{
	const sampler2DArray momentsTexture = sampler2DArray(shadowMaps.momentsTextureHandle);

	const vec2 texelPosSMS = NDC_XY_ZO * float(mapSize - 1u) + vec2(0.5f) + vec2(mapCoords.xy);
	const vec3 texCoords = vec3(texelPosSMS / vec2(textureSize(momentsTexture, 0).xy), float(mapCoords[2u]));
	
	const vec2 moments = texture(momentsTexture, texCoords).rg;
	
	vec3 result = vec3(ChebyshevUpperBound(moments, linearNormalizedDepth, lightBleedingAmount));
	
	if (!isTexelTransparent)
	{
		result *= texture(sampler2DArray(shadowMaps.colorTextureHandle), texCoords).rgb;
	}
	
	return result;
}

vec3 shadowMapsProccessEVSMShadow(
	in vec2 NDC_XY_ZO,
	in float linearNormalizedDepth,
	in uvec3 mapCoords,
	in uint mapSize,
	in float positiveExponent,
	in float negativeExponent,
	in float lightBleedingAmount,
	in bool isTexelTransparent)
{
	const sampler2DArray momentsTexture = sampler2DArray(shadowMaps.momentsTextureHandle);

	const vec2 texelPosSMS = NDC_XY_ZO * float(mapSize - 1u) + vec2(0.5f) + vec2(mapCoords.xy);
	const vec3 texCoords = vec3(texelPosSMS / vec2(textureSize(momentsTexture, 0).xy), float(mapCoords[2u]));
	
	const vec4 moments = texture(momentsTexture, texCoords);
	const vec2 momentsPos = moments.xy;
	const vec2 momentsNeg = moments.zw;
	
	const float sampleDepthPos = exp(positiveExponent * linearNormalizedDepth);
	const float sampleDepthNeg = -exp(-negativeExponent * linearNormalizedDepth);
	
	const float pMaxPos = ChebyshevUpperBound(momentsPos, sampleDepthPos, lightBleedingAmount);
	const float pMaxNeg = ChebyshevUpperBound(momentsNeg, sampleDepthNeg, lightBleedingAmount);
	
	vec3 result = vec3(min(pMaxPos, pMaxNeg));
	
	if (!isTexelTransparent)
	{
		//result *= texture(sampler2DArray(shadowMaps.colorTextureHandle), texCoords).rgb;
	}
	
	return result;
}

vec3 shadowMapsProccessHamburgerMSMShadow(
	in vec2 NDC_XY_ZO,
	in float linearNormalizedDepth,
	in uvec3 mapCoords,
	in uint mapSize,
	in float momentsBias,
	in bool isTexelTransparent)
{
	const sampler2DArray momentsTexture = sampler2DArray(shadowMaps.momentsTextureHandle);

	const vec2 texelPosSMS = NDC_XY_ZO * float(mapSize - 1u) + vec2(0.5f) + vec2(mapCoords.xy);
	const vec3 texCoords = vec3(texelPosSMS / vec2(textureSize(momentsTexture, 0).xy), float(mapCoords[2u]));
	
	const vec4 moments = texture(momentsTexture, texCoords);
	vec3 result = vec3(Hamburger4MSMShadowIntensity(moments, linearNormalizedDepth, momentsBias));
	
	if (!isTexelTransparent)
	{
		result *= texture(sampler2DArray(shadowMaps.colorTextureHandle), texCoords).rgb;
	}
	
	return result;
}

vec3 shadowMapsProccessHausdorffMSMShadow(
	in vec2 NDC_XY_ZO,
	in float linearNormalizedDepth,
	in uvec3 mapCoords,
	in uint mapSize,
	in float momentsBias,
	in bool isTexelTransparent)
{
	const sampler2DArray momentsTexture = sampler2DArray(shadowMaps.momentsTextureHandle);

	const vec2 texelPosSMS = NDC_XY_ZO * float(mapSize - 1u) + vec2(0.5f) + vec2(mapCoords.xy);
	const vec3 texCoords = vec3(texelPosSMS / vec2(textureSize(momentsTexture, 0).xy), float(mapCoords[2u]));
	
	const vec4 moments = texture(momentsTexture, texCoords);
	vec3 result = vec3(Hausdorff4MSMShadowIntensity(moments, linearNormalizedDepth, momentsBias));
	
	if (!isTexelTransparent)
	{
		result *= texture(sampler2DArray(shadowMaps.colorTextureHandle), texCoords).rgb;
	}
	
	return result;
}

float shadowMapsProcessVolumetricScattering(
	in vec3 NDC_ZO,
	in uvec3 mapCoords,
	in uint mapSize)
{
	const ivec2 texelPosSMS = ivec2(NDC_ZO.xy * float(mapSize - 1u) + vec2(0.5f)) + ivec2(mapCoords.xy);
	const ivec3 texCoords = ivec3(texelPosSMS, int(mapCoords[2u]));
	
	const float refDepth = texelFetch(sampler2DArray(shadowMaps.depthTextureHandle), texCoords, 0).r;
	return step(NDC_ZO[2u], refDepth);
}