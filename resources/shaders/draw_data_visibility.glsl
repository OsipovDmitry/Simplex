layout (std430) buffer ssbo_drawDataVisibilityBuffer { uint drawDataVisibility[]; };

bool drawDataVisibilityCheckPreviousFrame(in uint drawDataID, in uint drawDataVisisbilityFrameIndex)
{
	const int bitIndex = (drawDataVisisbilityFrameIndex == 0u) ? 0 : 1;
	return bool(bitfieldExtract(drawDataVisibility[drawDataID], bitIndex, 1));
}

void drawDataVisibilitySetCurrentFrame(in uint drawDataID, in bool value, in uint drawDataVisisbilityFrameIndex)
{
	const int bitIndex = (drawDataVisisbilityFrameIndex == 0u) ? 1 : 0;
	drawDataVisibility[drawDataID] = bitfieldInsert(drawDataVisibility[drawDataID], value ? 1u : 0u, bitIndex, 1);
}