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

uint skeletalAnimatedDataBonesTransfromsDataOffset(in uint skeletalAnimatedDataID)
{
	return skeletalAnimatedData[skeletalAnimatedDataID].bonesTransfromsDataOffset;
}

uint skeletalAnimatedDataLastUpdateTime(in uint skeletalAnimatedDataID)
{
	return skeletalAnimatedData[skeletalAnimatedDataID].lastUpdateTime;
}

uint skeletalAnimatedDataSetLastUpdateTime(in uint skeletalAnimatedDataID, in uint lastUpdateTime)
{
	return atomicExchange(skeletalAnimatedData[skeletalAnimatedDataID].lastUpdateTime, lastUpdateTime);
}