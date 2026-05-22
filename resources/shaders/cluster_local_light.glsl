#include<descriptions.glsl>

layout (std430) buffer ssbo_clusterLocalLightsBuffer { ClusterLocalLightDescription clusterLocalLights[]; };

uint clusterLocalLightLightID(in uint clusterLocalLightID)
{
	return clusterLocalLights[clusterLocalLightID].lightID;
}

void clusterLocalLightInitialize(in uint clusterLocalLightID, in uint lightID)
{
	clusterLocalLights[clusterLocalLightID] = makeClusterLocalLightDescription(lightID);
}