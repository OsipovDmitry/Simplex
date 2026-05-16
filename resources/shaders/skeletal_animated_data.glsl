#include<descriptions.glsl>

layout (std430) readonly buffer ssbo_skeletalAnimatedDataBuffer {
	SkeletalAnimatedDataDescription skeletalAnimatedData[];
};

uint skeletalAnimatedDataSkeletonID(in uint skeletalAnimatedDataID)
{
	return skeletalAnimatedData[skeletalAnimatedDataID].skeletonID;
}

uint skeletalAnimatedDataCurrentAnimationID(in uint skeletalAnimatedDataID)
{
	return skeletalAnimatedData[skeletalAnimatedDataID].currentAnimationID;
}

uint skeletalAnimatedDataBonesTransformsDataOffset(in uint skeletalAnimatedDataID)
{
	return skeletalAnimatedData[skeletalAnimatedDataID].bonesTransformsDataOffset;
}

uint skeletalAnimatedDataLastUpdateTime(in uint skeletalAnimatedDataID)
{
	return skeletalAnimatedData[skeletalAnimatedDataID].lastUpdateTime;
}

uint skeletalAnimatedDataSetLastUpdateTime(in uint skeletalAnimatedDataID, in uint lastUpdateTime)
{
	return atomicExchange(skeletalAnimatedData[skeletalAnimatedDataID].lastUpdateTime, lastUpdateTime);
}