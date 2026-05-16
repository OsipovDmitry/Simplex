#include<descriptions.glsl>

layout (std430) buffer ssbo_shadowDataBuffer { ShadowDataDescription shadowData[]; };

uint shadowDataDrawDataID(in uint shadowDataID)
{
	return shadowData[shadowDataID].drawDataID;
}

uint shadowDataShadowID(in uint shadowDataID)
{
	return shadowData[shadowDataID].shadowID;
}

uint shadowDataLayerID(in uint shadowDataID, in uint index)
{
	return shadowData[shadowDataID].layerIDs[index];
}

void shadowDataInitialize(in uint shadowDataID, in uint drawDataID, in uint shadowID, in uint layerIDs[6u])
{
	shadowData[shadowDataID] = makeShadowDataDescription(drawDataID, shadowID, layerIDs);
}
