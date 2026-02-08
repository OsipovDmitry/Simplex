#include<descriptions.glsl>

layout (std430) buffer ssbo_bonesTransformsDataBuffer { TransformDescription bonesTransformsData[]; };

Transform bonesTransformsDataBoneTransform(in uint bonesTransformsDataOffset, in uint boneID)
{
	return makeTransform(bonesTransformsData[bonesTransformsDataOffset + boneID]);
}

void bonesTransformsDataSetBoneTransform(in uint bonesTransformsDataOffset, in uint boneID, in Transform boneTransform)
{
	bonesTransformsData[bonesTransformsDataOffset + boneID] = makeTransformDescription(boneTransform);
}

