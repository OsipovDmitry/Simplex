#include<descriptions.glsl>

layout (std430) readonly buffer ssbo_mapsBuffer { TextureHandle mapsTextureHandles[]; };

TextureHandle mapTextureHandle(in uint mapID)
{
	return mapsTextureHandles[mapID];
}

bool isTextureHandleEmpty(in TextureHandle value)
{
	return (value[0u] == 0xFFFFFFFFu) && (value[1u] == 0xFFFFFFFFu);
}