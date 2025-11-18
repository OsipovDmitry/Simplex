struct BackgroundDescription
{
    vec4 environmentColorAndBlurPower;
    uint environmentMapID;
};

layout (std430) readonly buffer ssbo_backgroundBuffer { BackgroundDescription background; };

vec3 backgroundEnvironmentColor()
{
	return background.environmentColorAndBlurPower.rgb;
}

float backgroundBlurPower()
{
	return background.environmentColorAndBlurPower.a;
}

uint backgroundEnvironmentMapID()
{
	return background.environmentMapID;
}