#include<descriptions.glsl>

layout (std430) buffer ssbo_bonesTransformsDataBuffer { BonesTransformsDataDescription bonesTransformsData[]; };

Transform bonesTransformsDataBoneTransform(in uint bonesTransformsDataOffset, in uint boneID)
{
	return toTransform(bonesTransformsData[bonesTransformsDataOffset + boneID]);
}

void bonesTransformsDataSetBoneTransform(in uint bonesTransformsDataOffset, in uint boneID, in Transform boneTransform)
{
	bonesTransformsData[bonesTransformsDataOffset + boneID] = makeTransformDescription(boneTransform);
}

