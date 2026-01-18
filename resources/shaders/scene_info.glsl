#include<descriptions.glsl>

layout (std430) buffer ssbo_sceneInfoBuffer { SceneInfoDescription sceneInfo; };

uint sceneInfoDrawDataCount()
{
	return sceneInfo.drawDataCount;
}

uint sceneInfoLightsCount()
{
	return sceneInfo.lightsCount;
}

uint sceneInfoGenerateOpaqueCommandID()
{
	return atomicAdd(sceneInfo.opaqueCommandsCount, 1u);
}

uint sceneInfoGenerateTransparentCommandID()
{
	return atomicAdd(sceneInfo.transparentCommandsCount, 1u);
}

uint sceneInfoLightNodesMaxCount()
{
	return sceneInfo.lightNodesMaxCount;
}

uint sceneInfoGenerateLightNodeID()
{
	return atomicAdd(sceneInfo.lightNodesCount, 1u);
}

uint sceneInfoFirstGlobalLightNodeID()
{
	return sceneInfo.firstGlobalLightNodeID;
}

uint sceneInfoSetFirstGlobalLightNodeID(uint lightNodeID)
{
	return atomicExchange(sceneInfo.firstGlobalLightNodeID, lightNodeID);
}
