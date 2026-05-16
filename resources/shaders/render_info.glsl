#include<descriptions.glsl>

#include<math/clip_space.glsl>
#include<math/oriented_bounding_box.glsl>
#include<math/range.glsl>
#include<math/transform.glsl>

layout (std430) buffer ssbo_renderInfoBuffer { RenderInfoDescription renderInfo; };

uint renderInfoTime()
{
	return renderInfo.time;
}

float renderInfoDielectricSpecular()
{
	return renderInfo.dielectricSpecular;
}

OrientedBoundingBox renderInfoGlobalBoundingBox()
{
	return toOrientedBoundingBox(renderInfo.globalBoundingBox);
}

uint renderInfoDrawDataCount()
{
	return renderInfo.drawDataCount;
}

uint renderInfoSkeletalAnimatedDataCount()
{
	return renderInfo.skeletalAnimatedDataCount;
}

uint renderInfoShadowsCount()
{
	return renderInfo.shadowsCount;
}

uint renderInfoLightsCount()
{
	return renderInfo.lightsCount;
}

TextureHandle renderInfoShadowVarianceBluredTextureHandle()
{
	return renderInfo.shadowVarianceBluredTextureHandle;
}

TextureHandle renderInfoShadowColorBluredTextureHandle()
{
	return renderInfo.shadowColorBluredTextureHandle;
}

float renderInfoShadowBlurKernelSample(in uint radius)
{
	return renderInfo.shadowBlurKernel[radius];
}

uint renderInfoShadowBlurRadius()
{
	return renderInfo.shadowBlurRadius;
}

float renderInfoShadowLightBleedingAmount()
{
	return renderInfo.shadowLightBleedingAmount;
}

uint renderInfoShadowAtlasSize()
{
	return renderInfo.shadowAtlasSize;
}

uvec3 renderInfoClusterSize()
{
	return renderInfo.clusterSize.xyz;
}

Transform renderInfoViewTransform()
{
	return toTransform(renderInfo.viewTransform);
}

ClipSpace renderInfoClipSpace()
{
	return toClipSpace(renderInfo.clipSpace);
}

Range renderInfoCullPlaneLimits()
{
	return toRange(renderInfo.cullPlaneLimits);
}
