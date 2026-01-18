#include<math/bounding_box.glsl>
#include<descriptions.glsl>

layout (std430) buffer ssbo_clusterNodesBuffer { ClusterNodeDescription clusterNodes[]; };

ClusterNodeDescription makeClusterNodeDescription(in BoundingBox bb, in uint firstLightNodeID)
{
	ClusterNodeDescription desc;
	desc.boundingBoxMinPoint = vec4(bb.points[0u], 0.0f);
	desc.boundingBoxMaxPoint = vec4(bb.points[1u], 0.0f);
	desc.firstLightNodeID = firstLightNodeID;
	return desc;
}

BoundingBox clusterNodeBoundingBox(in uint clusterNodeID)
{
	return makeBoundingBox(
		clusterNodes[clusterNodeID].boundingBoxMinPoint.xyz,
		clusterNodes[clusterNodeID].boundingBoxMaxPoint.xyz);
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