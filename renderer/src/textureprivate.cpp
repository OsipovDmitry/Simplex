#include <algorithm>

#include "textureprivate.h"

namespace renderer {

GLenum toTextureGLType(TextureType val) {
	return textureTypeTable[castFromTextureType<size_t>(val)];
}
TextureType fromShaderGLType(GLenum val) {
	return castToTextureType(std::find(textureTypeTable.cbegin(), textureTypeTable.cend(), val) - textureTypeTable.cbegin());
}

GLenum toTextureInternalGLFormat(TextureInternalFormat val) {
	return textureInternalFormatTable[castFromTextureInternalFormat<size_t>(val)];
}
TextureInternalFormat fromTextureInternalGLFormat(GLenum val) {
	return castToTextureInternalFormat(std::find(textureInternalFormatTable.cbegin(), textureInternalFormatTable.cend(), val) - textureInternalFormatTable.cbegin());
}

GLenum toImageGLFormat(types::ImageFormat val)
{
	return imageFormatTable[types::castFromImageFormat<size_t>(val)];
}

types::ImageFormat fromImageGLFormat(GLenum val)
{
	return types::castToImageFormat(std::find(imageFormatTable.cbegin(), imageFormatTable.cend(), val) - imageFormatTable.cbegin());
}

GLenum toImageGLType(types::ImageType val)
{
	return imageTypeTable[types::castFromImageType<size_t>(val)];
}

types::ImageType fromImageGLType(GLenum val)
{
	return types::castToImageType(std::find(imageTypeTable.cbegin(), imageTypeTable.cend(), val) - imageTypeTable.cbegin());
}

GLenum toTextureCubemapGLSide(TextureCubemapSide val)
{
	return textureCubemapSideTable[castFromTextureCubemapSide<size_t>(val)];
}

TextureCubemapSide fromTextureCubemapGLSide(GLenum val)
{
	return castToTextureCubemapSide(std::find(textureCubemapSideTable.cbegin(), textureCubemapSideTable.cend(), val) - textureCubemapSideTable.cbegin());
}

}
