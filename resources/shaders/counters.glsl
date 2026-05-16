#include<descriptions.glsl>
#include<math/constants.glsl>
#include<math/range.glsl>

layout (std430) buffer ssbo_countersBuffer { CountersDescription counters; };

void countersReset()
{
    counters.ZRange = uvec2(floatBitsToUint(FLT_MAX), floatBitsToUint(0.0f));
    counters.firstGlobalLightNodeID = 0xFFFFFFFFu;
    counters.lightNodesCount = 0u;
	counters.skeletalAnimatedDataToUpdateCount = 0u;
    counters.shadowsToUpdateCount = 0u;
    counters.opaqueDrawDataRenderCommandsCount = 0u;
    counters.transparentDrawDataRenderCommandsCount = 0u;
	counters.shadowDataCount = 0u;
	counters.opaqueShadowDataRenderCommandsCount = 0u;
	counters.transparentShadowDataRenderCommandsCount = 0u;
}

Range countersZRange()
{
	return makeRange(uintBitsToFloat(counters.ZRange[0u]), uintBitsToFloat(counters.ZRange[1u]));
}

uint countersFirstGlobalLightNodeID()
{
	return counters.firstGlobalLightNodeID;
}

uint countersSetFirstGlobalLightNodeID(uint lightNodeID)
{
	return atomicExchange(counters.firstGlobalLightNodeID, lightNodeID);
}

uint countersGenerateLightNodeID()
{
	return atomicAdd(counters.lightNodesCount, 1u);
}

void countersExpandZRange(in Range nearFar)
{
	atomicMin(counters.ZRange[0u], floatBitsToUint(rangeStart(nearFar)));
	atomicMax(counters.ZRange[1u], floatBitsToUint(rangeEnd(nearFar)));
}

uint countersSkeletalAnimatedDataToUpdateCount()
{
	return counters.skeletalAnimatedDataToUpdateCount;
}

uint countersGenerateSkeletalAnimatedDataToUpdateID()
{
	return atomicAdd(counters.skeletalAnimatedDataToUpdateCount, 1u);
}

uint countersShadowsToUpdateCount()
{
	return counters.shadowsToUpdateCount;
}

uint countersGenerateShadowToUpdateID()
{
	return atomicAdd(counters.shadowsToUpdateCount, 1u);
}

uint countersGenerateOpaqueDrawDataRenderCommandID()
{
	return atomicAdd(counters.opaqueDrawDataRenderCommandsCount, 1u);
}

uint countersGenerateTransparentDrawDataRenderCommandID()
{
	return atomicAdd(counters.transparentDrawDataRenderCommandsCount, 1u);
}

uint countersGenerateShadowDataID()
{
	return atomicAdd(counters.shadowDataCount, 1u);
}

uint countersGenerateOpaqueShadowDataRenderCommandID()
{
	return atomicAdd(counters.opaqueShadowDataRenderCommandsCount, 1u);
}

uint countersGenerateTransparentShadowDataRenderCommandID()
{
	return atomicAdd(counters.transparentShadowDataRenderCommandsCount, 1u);
}
