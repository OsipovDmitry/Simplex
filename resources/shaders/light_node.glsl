struct LightNodeDescription
{
	uint lightID;
	uint nextID;
    uint padding[2u];
};
layout (std430) buffer ssbo_lightNodesBuffer { LightNodeDescription lightNodes[]; };

LightNodeDescription makeLightNode(in uint lightID, in uint nextID)
{
	LightNodeDescription desc;
	desc.lightID = lightID;
	desc.nextID = nextID;
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

void lightNodeInitialize(in uint lightNodeID, in uint lightID, in uint nextID)
{
	lightNodes[lightNodeID] = makeLightNode(lightID, nextID);
}