#include<descriptions.glsl>

#include<math/constants.glsl>
#include<math/gamma_correction.glsl>
#include<math/utils.glsl>

layout (std430) buffer ssbo_toneMappingBuffer { ToneMappingDescription toneMapping; };

void toneMappingAddColorToHistograms(in vec3 value)
{
	const float luminance = color2Luminance(value);
	const float logLuminance = (luminance < EPS) ?
		0.0f :
		clamp((log2(luminance) - toneMapping.minLogLuminance) * toneMapping.rangeInverseLogLuminance, 0.0f, 1.0f);
	const uint binIndex = uint(logLuminance * float(ToneMappingBinsCount - 1u));
	atomicAdd(toneMapping.bins[binIndex], 1);
}

void toneMappingClearHistograms()
{
	for (uint i = 0u; i < ToneMappingBinsCount; ++i)
		toneMapping.bins[i] = 0u;
}

void toneMappingCalculateExposure(in uint dt)
{
	uint pixelsTotalCount = 0u;
	for (uint i = 0u; i < ToneMappingBinsCount; ++i)
		pixelsTotalCount += toneMapping.bins[i];
	
	const uint startValidPixelsRange = uint(float(pixelsTotalCount) * toneMapping.pixelsFractionToTrimAtStart);
	const uint endValidPixelsRange = pixelsTotalCount - uint(float(pixelsTotalCount) * toneMapping.pixelsFractionToTrimAtEnd);
	
	float totalLuminance = 0.0f;
	uint startPixelsRange = 0u;
	for (uint i = 0u; i < ToneMappingBinsCount; ++i)
	{
		const uint endPixelsRange = startPixelsRange + toneMapping.bins[i];
		
		const uint startOverlappedRange = max(startValidPixelsRange, startPixelsRange);
		const uint endOverlappedRange = min(endValidPixelsRange, endPixelsRange);
		if (startOverlappedRange < endOverlappedRange)
			totalLuminance += float(i) * float(endOverlappedRange - startOverlappedRange);
		
		startPixelsRange = endPixelsRange;
	}

	const float averageBinIndex = totalLuminance / max(1u, endValidPixelsRange - startValidPixelsRange);
	const float currentLogLuminance = averageBinIndex / float(ToneMappingBinsCount - 1u);
	const float currentLuminance = exp2(currentLogLuminance / toneMapping.rangeInverseLogLuminance + toneMapping.minLogLuminance);
	const float currentClampedLuminance = clamp(currentLuminance, toneMapping.minClampLuminance, toneMapping.maxClampLuminance);
	
	const float prevLuminance = (toneMapping.luminancePrevFrame > 0.0f) ? toneMapping.luminancePrevFrame : toneMapping.baseLuminance;
	const float tau = (currentClampedLuminance > prevLuminance) ? toneMapping.tauLight : toneMapping.tauDark;
	const float adaptedLuminance = mix(prevLuminance, currentClampedLuminance, (1.0f - exp(-tau * float(dt) * 0.001f)));
	
	toneMapping.exposure = toneMapping.baseLuminance / max(EPS, adaptedLuminance);
	toneMapping.luminancePrevFrame = adaptedLuminance;
}

void toneMappingApplyExposure(inout vec3 color)
{
	color *= toneMapping.exposure;
}

void toneMappingApplyInvertedExposure(inout vec3 color)
{
	color /= toneMapping.exposure;
}

void toneMappingApplyACES(inout vec3 color)
{
	const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    color = (color * (a * color + b)) / (color * (c * color + d) + e);
	color = clamp(color, 0.0f, 1.0f);
}

void toneMappingApplyGammaCorrection(inout vec3 color)
{
	color = toSRGB(color);
}