#include<descriptions.glsl>

layout (std430) readonly buffer ssbo_backgroundBuffer { BackgroundDescription background; };

vec3 backgroundEnvironmentColor()
{
	return background.environmentColorAndBlurPower.rgb;
}

Quat backgroundRotation()
{
	return makeQuat(background.rotation);
}

float backgroundBlurPower()
{
	return background.environmentColorAndBlurPower.a;
}

uint backgroundEnvironmentMapID()
{
	return background.environmentMapID;
}