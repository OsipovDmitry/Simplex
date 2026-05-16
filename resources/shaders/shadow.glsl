#include<descriptions.glsl>

#include<math/range.glsl>

layout (std430) readonly buffer ssbo_shadowsBuffer { ShadowDescription shadows[]; };

uint shadowMapSize(in uint shadowID)
{
	return shadows[shadowID].mapSize;
}

Range shadowCullPlaneLimits(in uint shadowID)
{
	return toRange(shadows[shadowID].cullPlaneLimits);
}

uint shadowLayersCount(in uint shadowID)
{
	return bitfieldExtract(shadows[shadowID].flags, 0, 3);
}

uint shadowTransformsDataOffset(in uint shadowID)
{
	return shadows[shadowID].transformsDataOffset;
}