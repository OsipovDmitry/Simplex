#include<math/transform.glsl>
#include<descriptions.glsl>

layout (std430) readonly buffer ssbo_drawDataBuffer {
	DrawDataDescription drawData[];
};

Transform drawDataTransform(in uint drawDataID)
{
	return makeTransform(drawData[drawDataID].transform);
}

uint drawDataDrawableID(in uint drawDataID)
{
	return drawData[drawDataID].drawableID;
}