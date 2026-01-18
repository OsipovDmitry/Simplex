#include<descriptions.glsl>

layout (std430) readonly buffer ssbo_drawablesBuffer { DrawableDescription drawables[]; };

uint drawableMeshID(in uint drawableID)
{
	return drawables[drawableID].meshID;
}

uint drawableMaterialID(in uint drawableID)
{
	return drawables[drawableID].materialID;
}