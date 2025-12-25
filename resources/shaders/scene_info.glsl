struct SceneInfoDescription
{
	uvec4 clusterSize;
    uvec2 ZRange;
    uint drawDataCount;
    uint lightsCount;
    uint opaqueCommandsCount;
    uint transparentCommandsCount;
    uint OITNodesMaxCount;
    uint OITNodesCount;
	uint lightNodesMaxCount;
	uint lightNodesCount;
    uint firstGlobalLightNodeID;
    uint padding[2u];
};

layout (std430) buffer ssbo_sceneInfoBuffer { SceneInfoDescription sceneInfo; };

void sceneInfoExpandZRange(in vec2 nearFar)
{
	atomicMin(sceneInfo.ZRange[0u], floatBitsToUint(nearFar[0u]));
	atomicMax(sceneInfo.ZRange[1u], floatBitsToUint(nearFar[1u]));
}

uvec3 sceneInfoClusterSize()
{
	return sceneInfo.clusterSize.xyz;
}

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

uint sceneInfoOITNodesMaxCount()
{
	return sceneInfo.OITNodesMaxCount;
}

uint sceneInfoGenerateOITNodeID()
{
	return atomicAdd(sceneInfo.OITNodesCount, 1u);
}

uint sceneInfoLightNodesMaxCount()
{
	return sceneInfo.lightNodesMaxCount;
}

uint sceneInfoGenerateLightNodeID()
{
	return atomicAdd(sceneInfo.lightNodesCount, 1u);
}

uint sceneInfoSetFirstGlobalLightNodeID(uint lightNodeID)
{
	return atomicExchange(sceneInfo.firstGlobalLightNodeID, lightNodeID);
}
