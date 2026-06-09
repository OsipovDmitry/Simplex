#include<descriptions.glsl>

#include<math/transform.glsl>

layout (std430) buffer ssbo_shadowsToUpdateBuffer { ShadowToUpdateDescription shadowsToUpdate[]; };

uint shadowToUpdateShadowID(in uint shadowToUpdateID)
{
	return shadowsToUpdate[shadowToUpdateID].shadowID;
}

void shadowToUpdateSetShadowID(in uint shadowToUpdateID, in uint shadowID)
{
	shadowsToUpdate[shadowToUpdateID].shadowID = shadowID;
}