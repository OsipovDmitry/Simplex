#include<descriptions.glsl>
#include<math/bounding_box.glsl>

layout (std430) buffer ssbo_clusterNodesBuffer { ClusterNodeDescription clusterNodes[]; };

BoundingBox clusterNodeBoundingBox(in uint clusterNodeID)
{
	return toBoundingBox(clusterNodes[clusterNodeID].boundingBox);
}

uint clusterNodeFirstLightNodeID(in uint clusterNodeID)
{
	return clusterNodes[clusterNodeID].firstLightNodeID;
}

void clusterNodeInitialize(in uint clusterNodeID, in BoundingBox bb)
{
	clusterNodes[clusterNodeID] = makeClusterNodeDescription(bb, 0xFFFFFFFFu);
}

uint clusterNodeSetFirstLightNodeID(in uint clusterNodeID, uint lightNodeID)
{
	return atomicExchange(clusterNodes[clusterNodeID].firstLightNodeID, lightNodeID);
}