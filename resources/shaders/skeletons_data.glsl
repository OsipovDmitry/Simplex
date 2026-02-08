#include<descriptions.glsl>

layout (std430) readonly buffer ssbo_skeletonsDataBuffer { float skeletonsData[]; };

uint skeletonsDataSkeletonBonesCount(in uint skeletonsDataOffset)
{
	return floatBitsToUint(skeletonsData[skeletonsDataOffset + 0u]);
}

uint skeletonsDataSkeletonRootBoneID(in uint skeletonsDataOffset)
{
	return floatBitsToUint(skeletonsData[skeletonsDataOffset + 1u]);
}

uint skeletonsDataSkeletonAnimationsCount(in uint skeletonsDataOffset)
{
	return floatBitsToUint(skeletonsData[skeletonsDataOffset + 2u]);
}

uint skeletonsDataSkeletonBoneDataOffset(in uint skeletonsDataOffset, in uint boneID)
{
	return floatBitsToUint(skeletonsData[skeletonsDataOffset + 3u + boneID]);
}

uint skeletonsDataSkeletonAnimationDataOffset(in uint skeletonsDataOffset, in uint animationID)
{
	return floatBitsToUint(skeletonsData[skeletonsDataOffset + 3u + skeletonsDataSkeletonBonesCount(skeletonsDataOffset) + animationID]);
}

Transform skeletonsDataSkeletonBoneOffsetTransform(in uint skeletonsDataOffset, in uint boneID)
{
	const uint boneDataOffset = skeletonsDataSkeletonBoneDataOffset(skeletonsDataOffset, boneID);
	const uint boneOffsetTransformOffset = skeletonsDataOffset + boneDataOffset + 0u;
	
	const QuatDescription rotation = QuatDescription(
		skeletonsData[boneOffsetTransformOffset + 0u],
		skeletonsData[boneOffsetTransformOffset + 1u],
		skeletonsData[boneOffsetTransformOffset + 2u],
		skeletonsData[boneOffsetTransformOffset + 3u]);
	
	const vec4 translationAndScale = vec4(
		skeletonsData[boneOffsetTransformOffset + 4u],
		skeletonsData[boneOffsetTransformOffset + 5u],
		skeletonsData[boneOffsetTransformOffset + 6u],
		skeletonsData[boneOffsetTransformOffset + 7u]);
		
	return makeTransform(TransformDescription(rotation, translationAndScale));	
}

Transform skeletonsDataSkeletonBoneTransform(in uint skeletonsDataOffset, in uint boneID)
{
	const uint boneDataOffset = skeletonsDataSkeletonBoneDataOffset(skeletonsDataOffset, boneID);
	const uint boneTransformOffset = skeletonsDataOffset + boneDataOffset + 8u;
	
	const QuatDescription rotation = QuatDescription(
		skeletonsData[boneTransformOffset + 0u],
		skeletonsData[boneTransformOffset + 1u],
		skeletonsData[boneTransformOffset + 2u],
		skeletonsData[boneTransformOffset + 3u]);
	
	const vec4 translationAndScale = vec4(
		skeletonsData[boneTransformOffset + 4u],
		skeletonsData[boneTransformOffset + 5u],
		skeletonsData[boneTransformOffset + 6u],
		skeletonsData[boneTransformOffset + 7u]);
		
	return makeTransform(TransformDescription(rotation, translationAndScale));	
}

uint skeletonsDataSkeletonBoneParentID(in uint skeletonsDataOffset, in uint boneID)
{
	const uint boneDataOffset = skeletonsDataSkeletonBoneDataOffset(skeletonsDataOffset, boneID);
	const uint boneParentIDOffset = skeletonsDataOffset + boneDataOffset + 16u;
	
	return floatBitsToUint(skeletonsData[boneParentIDOffset]);	
}

uint skeletonsDataSkeletonBoneChildrenCount(in uint skeletonsDataOffset, in uint boneID)
{
	const uint boneDataOffset = skeletonsDataSkeletonBoneDataOffset(skeletonsDataOffset, boneID);
	const uint boneChildrenCountOffset = skeletonsDataOffset + boneDataOffset + 17u;
	
	return floatBitsToUint(skeletonsData[boneChildrenCountOffset]);	
}

uint skeletonsDataSkeletonBoneChildID(in uint skeletonsDataOffset, in uint boneID, in uint childIndex)
{
	const uint boneDataOffset = skeletonsDataSkeletonBoneDataOffset(skeletonsDataOffset, boneID);
	const uint boneChildrenIDsOffset = skeletonsDataOffset + boneDataOffset + 18u;
	
	return floatBitsToUint(skeletonsData[boneChildrenIDsOffset + childIndex]);	
}

uint skeletonsDataSkeletonAnimationDuration(in uint skeletonsDataOffset, in uint animationID)
{
	const uint animationDataOffset = skeletonsDataSkeletonAnimationDataOffset(skeletonsDataOffset, animationID);
	const uint animationDurationOffset = skeletonsDataOffset + animationDataOffset + 0u;
	
	return floatBitsToUint(skeletonsData[animationDurationOffset]);	
}

uint skeletonsDataSkeletonAnimationTicksPerSecond(in uint skeletonsDataOffset, in uint animationID)
{
	const uint animationDataOffset = skeletonsDataSkeletonAnimationDataOffset(skeletonsDataOffset, animationID);
	const uint animationTicksPerSecondOffset = skeletonsDataOffset + animationDataOffset + 1u;
	
	return floatBitsToUint(skeletonsData[animationTicksPerSecondOffset]);	
}

uint skeletonsDataSkeletonAnimationChannelsCount(in uint skeletonsDataOffset, in uint animationID)
{
	const uint animationDataOffset = skeletonsDataSkeletonAnimationDataOffset(skeletonsDataOffset, animationID);
	const uint animationChannelsCountOffset = skeletonsDataOffset + animationDataOffset + 2u;
	
	return floatBitsToUint(skeletonsData[animationChannelsCountOffset]);	
}

uint skeletonsDataSkeletonAnimationChannelOffset(in uint skeletonsDataOffset, in uint animationID, in uint channelIndex)
{
	const uint animationDataOffset = skeletonsDataSkeletonAnimationDataOffset(skeletonsDataOffset, animationID);
	const uint animationChannelsOffsetsOffset = skeletonsDataOffset + animationDataOffset + 3u;
	
	return floatBitsToUint(skeletonsData[animationChannelsOffsetsOffset + channelIndex]);	
}

uint skeletonsDataSkeletonAnimationChannelBoneID(in uint skeletonsDataOffset, in uint animationID, in uint channelIndex)
{
	const uint animationChannelOffset = skeletonsDataSkeletonAnimationChannelOffset(skeletonsDataOffset, animationID, channelIndex);
	const uint animationChannelBoneIDOffset = skeletonsDataOffset + animationChannelOffset + 0u;
	
	return floatBitsToUint(skeletonsData[animationChannelBoneIDOffset]);	
}

uint skeletonsDataSkeletonAnimationChannelScaleKeysCount(in uint skeletonsDataOffset, in uint animationID, in uint channelIndex)
{
	const uint animationChannelOffset = skeletonsDataSkeletonAnimationChannelOffset(skeletonsDataOffset, animationID, channelIndex);
	const uint animationChannelScaleKeysCountOffset = skeletonsDataOffset + animationChannelOffset + 1u;
	
	return floatBitsToUint(skeletonsData[animationChannelScaleKeysCountOffset]);	
}

uint skeletonsDataSkeletonAnimationChannelRotationKeysCount(in uint skeletonsDataOffset, in uint animationID, in uint channelIndex)
{
	const uint animationChannelOffset = skeletonsDataSkeletonAnimationChannelOffset(skeletonsDataOffset, animationID, channelIndex);
	const uint animationChannelRotationKeysCountOffset = skeletonsDataOffset + animationChannelOffset + 2u;
	
	return floatBitsToUint(skeletonsData[animationChannelRotationKeysCountOffset]);	
}

uint skeletonsDataSkeletonAnimationChannelTranslationKeysCount(in uint skeletonsDataOffset, in uint animationID, in uint channelIndex)
{
	const uint animationChannelOffset = skeletonsDataSkeletonAnimationChannelOffset(skeletonsDataOffset, animationID, channelIndex);
	const uint animationChannelTranslationKeysCountOffset = skeletonsDataOffset + animationChannelOffset + 3u;
	
	return floatBitsToUint(skeletonsData[animationChannelTranslationKeysCountOffset]);	
}

uint skeletonsDataSkeletonAnimationChannelScaleKeyTime(
	in uint skeletonsDataOffset,
	in uint animationID,
	in uint channelIndex,
	in uint keyIndex)
{
	const uint animationChannelOffset = skeletonsDataSkeletonAnimationChannelOffset(skeletonsDataOffset, animationID, channelIndex);
	const uint animationChannelScaleKeysOffset = skeletonsDataOffset + animationChannelOffset + 4u;
	const uint animationChannelScaleKeyOffset = animationChannelScaleKeysOffset + keyIndex * 2u;
	
	return floatBitsToUint(skeletonsData[animationChannelScaleKeyOffset + 0u]);
}

float skeletonsDataSkeletonAnimationChannelScaleKeyValue(
	in uint skeletonsDataOffset,
	in uint animationID,
	in uint channelIndex,
	in uint keyIndex)
{
	const uint animationChannelOffset = skeletonsDataSkeletonAnimationChannelOffset(skeletonsDataOffset, animationID, channelIndex);
	const uint animationChannelScaleKeysOffset = skeletonsDataOffset + animationChannelOffset + 4u;
	const uint animationChannelScaleKeyOffset = animationChannelScaleKeysOffset + keyIndex * 2u;
	
	return skeletonsData[animationChannelScaleKeyOffset + 1u];
}

uint skeletonsDataSkeletonAnimationChannelRotationKeyTime(
	in uint skeletonsDataOffset,
	in uint animationID,
	in uint channelIndex,
	in uint keyIndex)
{
	const uint animationChannelOffset = skeletonsDataSkeletonAnimationChannelOffset(skeletonsDataOffset, animationID, channelIndex);
	const uint animationChannelRotationKeysOffset =
		skeletonsDataOffset + animationChannelOffset + 4u +
		skeletonsDataSkeletonAnimationChannelScaleKeysCount(skeletonsDataOffset, animationID, channelIndex) * 2u;
	const uint animationChannelRotationKeyOffset = animationChannelRotationKeysOffset + keyIndex * 5u;
	
	return floatBitsToUint(skeletonsData[animationChannelRotationKeyOffset + 0u]);
}

Quat skeletonsDataSkeletonAnimationChannelRotationKeyValue(
	in uint skeletonsDataOffset,
	in uint animationID,
	in uint channelIndex,
	in uint keyIndex)
{
	const uint animationChannelOffset = skeletonsDataSkeletonAnimationChannelOffset(skeletonsDataOffset, animationID, channelIndex);
	const uint animationChannelRotationKeysOffset =
		skeletonsDataOffset + animationChannelOffset + 4u +
		skeletonsDataSkeletonAnimationChannelScaleKeysCount(skeletonsDataOffset, animationID, channelIndex) * 2u;
	const uint animationChannelRotationKeyOffset = animationChannelRotationKeysOffset + keyIndex * 5u;
	
	return Quat(
		skeletonsData[animationChannelRotationKeyOffset + 1u],
		skeletonsData[animationChannelRotationKeyOffset + 2u],
		skeletonsData[animationChannelRotationKeyOffset + 3u],
		skeletonsData[animationChannelRotationKeyOffset + 4u]);
}

uint skeletonsDataSkeletonAnimationChannelTranslationKeyTime(
	in uint skeletonsDataOffset,
	in uint animationID,
	in uint channelIndex,
	in uint keyIndex)
{
	const uint animationChannelOffset = skeletonsDataSkeletonAnimationChannelOffset(skeletonsDataOffset, animationID, channelIndex);
	const uint animationChannelTranslationKeysOffset =
		skeletonsDataOffset + animationChannelOffset + 4u +
		skeletonsDataSkeletonAnimationChannelScaleKeysCount(skeletonsDataOffset, animationID, channelIndex) * 2u +
		skeletonsDataSkeletonAnimationChannelRotationKeysCount(skeletonsDataOffset, animationID, channelIndex) * 5u;
	const uint animationChannelTranslationKeyOffset = animationChannelTranslationKeysOffset + keyIndex * 4u;
	
	return floatBitsToUint(skeletonsData[animationChannelTranslationKeyOffset + 0u]);
}

vec3 skeletonsDataSkeletonAnimationChannelTranslationKeyValue(
	in uint skeletonsDataOffset,
	in uint animationID,
	in uint channelIndex,
	in uint keyIndex)
{
	const uint animationChannelOffset = skeletonsDataSkeletonAnimationChannelOffset(skeletonsDataOffset, animationID, channelIndex);
	const uint animationChannelTranslationKeysOffset =
		skeletonsDataOffset + animationChannelOffset + 4u +
		skeletonsDataSkeletonAnimationChannelScaleKeysCount(skeletonsDataOffset, animationID, channelIndex) * 2u +
		skeletonsDataSkeletonAnimationChannelRotationKeysCount(skeletonsDataOffset, animationID, channelIndex) * 5u;
	const uint animationChannelTranslationKeyOffset = animationChannelTranslationKeysOffset + keyIndex * 4u;
	
	return vec3(
		skeletonsData[animationChannelTranslationKeyOffset + 1u],
		skeletonsData[animationChannelTranslationKeyOffset + 2u],
		skeletonsData[animationChannelTranslationKeyOffset + 3u]);
}

