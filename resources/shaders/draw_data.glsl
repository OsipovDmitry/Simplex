#include<math/transform.glsl>

struct DrawDataBufferReservedData
{
	uint count;
	uint padding[3u];
};

struct DrawDataDescription
{
	Transform transform;
	uint drawableID;
	uint padding[3u];
};

layout (std430) readonly buffer ssbo_drawDataBuffer {
	DrawDataBufferReservedData drawDataBufferReservedData;
	DrawDataDescription drawData[];
};

uint drawDataCount()
{
	return drawDataBufferReservedData.count;
}

Transform drawDataTransform(in uint drawDataID)
{
	return drawData[drawDataID].transform;
}

uint drawDataDrawableID(in uint drawDataID)
{
	return drawData[drawDataID].drawableID;
}