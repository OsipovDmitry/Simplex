#include<descriptions.glsl>

layout (std430)  buffer ssbo_bonesDataBuffer { TransformDescription bonesData[]; };

Transform bonesDataBoneTransform(in uint boneID)
{
	return makeTransform(bonesData[boneID]);
}

