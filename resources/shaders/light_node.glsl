#include<descriptions.glsl>

layout (std430) buffer ssbo_lightNodesBuffer { LightNodeDescription lightNodes[]; };

LightNodeDescription makeLightNodeDescription(in uint lightID, in uint nextLightNodeID)
{
	LightNodeDescription desc;
	desc.lightID = lightID;
	desc.nextID = nextLightNodeID;
	return desc;
}

uint lightNodeLightID(in uint lightNodeID)
{
	return lightNodes[lightNodeID].lightID;
}

uint lightNodeNextID(in uint lightNodeID)
{
	return lightNodes[lightNodeID].nextID;
}

void lightNodeInitialize(in uint lightNodeID, in uint lightID, in uint nextLightNodeID)
{
	lightNodes[lightNodeID] = makeLightNodeDescription(lightID, nextLightNodeID);
}