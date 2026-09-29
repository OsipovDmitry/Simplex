#include<descriptions.glsl>

layout (std430) buffer ssbo_visibleClusterNodesBuffer { uint visibleClusterNodes[]; };

uint visibleClusterNodeClusterNodeID(in uint visibleClusterNodeID)
{
	return visibleClusterNodes[visibleClusterNodeID];
}

void visibleClusterNodeSetClusterNodeID(in uint visibleClusterNodeID, in uint clusterNodeID)
{
	visibleClusterNodes[visibleClusterNodeID] = clusterNodeID;
}