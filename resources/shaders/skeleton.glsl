#include<skeletons_data.glsl>

layout (std430) readonly buffer ssbo_skeletonsBuffer { SkeletonDescription skeletons[]; };

uint skeletonBonesCount(in uint skeletonID)
{
	return skeletonsDataSkeletonBonesCount(skeletons[skeletonID].dataOffset);
}

uint skeletonRootBoneID(in uint skeletonID)
{
	return skeletonsDataSkeletonRootBoneID(skeletons[skeletonID].dataOffset);
}

uint skeletonAnimationsCount(in uint skeletonID)
{
	return skeletonsDataSkeletonAnimationsCount(skeletons[skeletonID].dataOffset);
}

Transform skeletonBoneOffsetTransform(in uint skeletonID, in uint boneID)
{
	return skeletonsDataSkeletonBoneOffsetTransform(skeletons[skeletonID].dataOffset, boneID);
}

Transform skeletonBoneTransform(in uint skeletonID, in uint boneID)
{
	return skeletonsDataSkeletonBoneTransform(skeletons[skeletonID].dataOffset, boneID);
}

uint skeletonBoneParentID(in uint skeletonID, in uint boneID)
{
	return skeletonsDataSkeletonBoneParentID(skeletons[skeletonID].dataOffset, boneID);
}

uint skeletonBoneChildrenCount(in uint skeletonID, in uint boneID)
{
	return skeletonsDataSkeletonBoneChildrenCount(skeletons[skeletonID].dataOffset, boneID);
}

uint skeletonBoneChildID(in uint skeletonID, in uint boneID, in uint childIndex)
{
	return skeletonsDataSkeletonBoneChildID(skeletons[skeletonID].dataOffset, boneID, childIndex);
}

uint skeletonAnimationDuration(in uint skeletonID, in uint animationID)
{
	return skeletonsDataSkeletonAnimationDuration(skeletons[skeletonID].dataOffset, animationID);
}

uint skeletonAnimationTicksPerSecond(in uint skeletonID, in uint animationID)
{
	return skeletonsDataSkeletonAnimationTicksPerSecond(skeletons[skeletonID].dataOffset, animationID);
}

uint skeletonAnimationChannelsCount(in uint skeletonID, in uint animationID)
{
	return skeletonsDataSkeletonAnimationChannelsCount(skeletons[skeletonID].dataOffset, animationID);
}

uint skeletonAnimationChannelBoneID(in uint skeletonID, in uint animationID, in uint channelIndex)
{
	return skeletonsDataSkeletonAnimationChannelBoneID(skeletons[skeletonID].dataOffset, animationID, channelIndex);	
}

uint skeletonAnimationFindChannelIndexForBoneID(in uint skeletonID, in uint animationID, in uint boneID)
{
	const uint channelsCount = skeletonAnimationChannelsCount(skeletonID, animationID);
	for (uint channelIndex = 0u; channelIndex < channelsCount; ++channelIndex)
	{
		if (skeletonAnimationChannelBoneID(skeletonID, animationID, channelIndex) == boneID)
			return channelIndex;
	}
	return 0xFFFFFFFFu;
}

Transform skeletonAnimationCalculateChannelTransform(in uint skeletonID, in uint animationID, in uint channelIndex, in uint time)
{
	const uint skeletonsDataOffset = skeletons[skeletonID].dataOffset;
	
	const uint timeInTicks =
		uint((float(skeletonAnimationTicksPerSecond(skeletonID, animationID)) / 1000.0) * float(time)) %
		skeletonAnimationDuration(skeletonID, animationID);
	
	float scale = 1.0f;
	Quat rotation = Quat(0.0f, 0.0f, 0.0f, 1.0f);
	vec3 translation = vec3(0.0f);
	
	const uint scaleKeysCount = skeletonsDataSkeletonAnimationChannelScaleKeysCount(skeletonsDataOffset, animationID, channelIndex);
	if (scaleKeysCount > 1u)
	{
		uint index1;
		uint timeInTicks1;
		for (index1 = 1u; index1 < scaleKeysCount; ++index1)
		{
			timeInTicks1 = skeletonsDataSkeletonAnimationChannelScaleKeyTime(skeletonsDataOffset, animationID, channelIndex, index1);
			if (timeInTicks < timeInTicks1)
				break;
		}
		const uint index0 = index1 - 1u;
		const uint timeInTicks0 = skeletonsDataSkeletonAnimationChannelScaleKeyTime(skeletonsDataOffset, animationID, channelIndex, index0);
		
		scale = mix(
			skeletonsDataSkeletonAnimationChannelScaleKeyValue(skeletonsDataOffset, animationID, channelIndex, index0),
			skeletonsDataSkeletonAnimationChannelScaleKeyValue(skeletonsDataOffset, animationID, channelIndex, index1),
			float(timeInTicks - timeInTicks0) / float(timeInTicks1 - timeInTicks0));
		
	}
	else if (scaleKeysCount == 1u)
	{
		scale = skeletonsDataSkeletonAnimationChannelScaleKeyValue(skeletonsDataOffset, animationID, channelIndex, 0u);
	}
	
	const uint rotationKeysCount = skeletonsDataSkeletonAnimationChannelRotationKeysCount(skeletonsDataOffset, animationID, channelIndex);
	if (rotationKeysCount > 1u)
	{
		uint index1;
		uint timeInTicks1;
		for (index1 = 1u; index1 < rotationKeysCount; ++index1)
		{
			timeInTicks1 = skeletonsDataSkeletonAnimationChannelRotationKeyTime(skeletonsDataOffset, animationID, channelIndex, index1);
			if (timeInTicks < timeInTicks1)
				break;
		}
		const uint index0 = index1 - 1u;
		const uint timeInTicks0 = skeletonsDataSkeletonAnimationChannelRotationKeyTime(skeletonsDataOffset, animationID, channelIndex, index0);
		
		rotation = slerp(
			skeletonsDataSkeletonAnimationChannelRotationKeyValue(skeletonsDataOffset, animationID, channelIndex, index0),
			skeletonsDataSkeletonAnimationChannelRotationKeyValue(skeletonsDataOffset, animationID, channelIndex, index1),
			float(timeInTicks - timeInTicks0) / float(timeInTicks1 - timeInTicks0));
		
	}
	else if (rotationKeysCount == 1u)
	{
		rotation = skeletonsDataSkeletonAnimationChannelRotationKeyValue(skeletonsDataOffset, animationID, channelIndex, 0u);
	}
	
	const uint translationKeysCount = skeletonsDataSkeletonAnimationChannelTranslationKeysCount(skeletonsDataOffset, animationID, channelIndex);
	if (translationKeysCount > 1u)
	{
		uint index1;
		uint timeInTicks1;
		for (index1 = 1u; index1 < translationKeysCount; ++index1)
		{
			timeInTicks1 = skeletonsDataSkeletonAnimationChannelTranslationKeyTime(skeletonsDataOffset, animationID, channelIndex, index1);
			if (timeInTicks < timeInTicks1)
				break;
		}
		const uint index0 = index1 - 1u;
		const uint timeInTicks0 = skeletonsDataSkeletonAnimationChannelTranslationKeyTime(skeletonsDataOffset, animationID, channelIndex, index0);
		
		translation = mix(
			skeletonsDataSkeletonAnimationChannelTranslationKeyValue(skeletonsDataOffset, animationID, channelIndex, index0),
			skeletonsDataSkeletonAnimationChannelTranslationKeyValue(skeletonsDataOffset, animationID, channelIndex, index1),
			float(timeInTicks - timeInTicks0) / float(timeInTicks1 - timeInTicks0));
		
	}
	else if (translationKeysCount == 1u)
	{
		translation = skeletonsDataSkeletonAnimationChannelTranslationKeyValue(skeletonsDataOffset, animationID, channelIndex, 0u);
	}
	
	return makeTransform(scale, rotation, translation);
}
