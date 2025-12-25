#include<math/transform.glsl>

struct DrawDataDescription
{
	Transform transform;
	uint drawableID;
	uint padding[3u];
};

layout (std430) readonly buffer ssbo_drawDataBuffer {
	DrawDataDescription drawData[];
};

Transform drawDataTransform(in uint drawDataID)
{
	return drawData[drawDataID].transform;
}

uint drawDataDrawableID(in uint drawDataID)
{
	return drawData[drawDataID].drawableID;
}