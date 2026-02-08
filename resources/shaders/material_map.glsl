#include<descriptions.glsl>

layout (std430) readonly buffer ssbo_materialMapsBuffer { TextureHandle materialMapsTextureHandles[]; };

TextureHandle materialMapTextureHandle(in uint materialMapID)
{
	return materialMapsTextureHandles[materialMapID];
}

bool isTextureHandleEmpty(in TextureHandle value)
{
	return (value[0u] == 0xFFFFFFFFu) && (value[1u] == 0xFFFFFFFFu);
}