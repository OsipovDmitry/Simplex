#include<descriptions.glsl>

layout (std430) readonly buffer ssbo_materialMapsBuffer { TextureHandle materialMapsTextureHandles[]; };

TextureHandle materialMapTextureHandle(in uint materialMapID)
{
	return materialMapsTextureHandles[materialMapID];
}