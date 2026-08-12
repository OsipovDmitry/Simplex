#include<descriptions.glsl>

layout (std430) buffer ssbo_hierarchicalZBuffer { HierarchicalZBufferDescription hierarchicalZ; };
layout (std430) buffer ssbo_hierarchicalZPingVisibilityBuffer { uint hierarchicalZPingVisibility[]; };
layout (std430) buffer ssbo_hierarchicalZPongVisibilityBuffer { uint hierarchicalZPongVisibility[]; };

void hierarchicalZReset()
{
	hierarchicalZ.pingPongVisibilityIndex = 1u - hierarchicalZ.pingPongVisibilityIndex;
    hierarchicalZ.earlyDrawDataCount = 0u;
    hierarchicalZ.opaqueDrawDataCount = 0u;
    hierarchicalZ.transparentDrawDataCount = 0u;
}

bool hierarchicalZWasDrawDataVisible(in uint drawDataID)
{
	return (hierarchicalZ.pingPongVisibilityIndex == 0u) ?
		bool(hierarchicalZPingVisibility[drawDataID]) :
		bool(hierarchicalZPongVisibility[drawDataID]);
}

void hierarchicalZSetDrawDataVisible(in uint drawDataID, in bool value)
{
	if (hierarchicalZ.pingPongVisibilityIndex == 0u)
		hierarchicalZPongVisibility[drawDataID] = uint(value);
	else
		hierarchicalZPingVisibility[drawDataID] = uint(value);
}

uint hierarchicalZGenerateEarlyDrawDataRenderCommandID()
{
	return atomicAdd(hierarchicalZ.earlyDrawDataCount, 1u);
}

uint hierarchicalZGenerateOpaqueDrawDataRenderCommandID()
{
	return atomicAdd(hierarchicalZ.opaqueDrawDataCount, 1u);
}

uint hierarchicalZGenerateTransparentDrawDataRenderCommandID()
{
	return atomicAdd(hierarchicalZ.transparentDrawDataCount, 1u);
}