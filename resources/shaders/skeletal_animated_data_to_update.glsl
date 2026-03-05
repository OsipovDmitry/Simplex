#include<descriptions.glsl>

layout (std430) buffer ssbo_skeletalAnimatedDataToUpdateBuffer {
	SkeletalAnimatedDataToUpdateDescription skeletalAnimatedDataToUpdate[];
};

uint skeletalAnimatedDataToUpdateSkeletalAnimatedDataID(in uint skeletalAnimatedDataToUpdateID)
{
	return skeletalAnimatedDataToUpdate[skeletalAnimatedDataToUpdateID].skeletalAnimatedDataID;
}

void skeletalAnimatedDataToUpdateSetSkeletalAnimatedDataID(in uint skeletalAnimatedDataToUpdateID, in uint skeletalAnimatedDataID)
{
	skeletalAnimatedDataToUpdate[skeletalAnimatedDataToUpdateID].skeletalAnimatedDataID = skeletalAnimatedDataID;
}