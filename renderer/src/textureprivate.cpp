#include <algorithm>

#include "textureprivate.h"

namespace renderer {

GLenum toTextureGLType(TextureType val)
{
	return textureTypeTable[castFromTextureType<size_t>(val)];
}

TextureType fromShaderGLType(GLenum val)
{
	return castToTextureType(std::find(textureTypeTable.cbegin(), textureTypeTable.cend(), val) - textureTypeTable.cbegin());
}

GLenum toTextureInternalGLFormat(TextureInternalFormat val)
{
	return textureInternalFormatTable[castFromTextureInternalFormat<size_t>(val)];
}

TextureInternalFormat fromTextureInternalGLFormat(GLenum val)
{
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

GLenum toTextureMinificationGLFilter(TextureMinificationFilter val)
{
	return textureMinificationFilterTable[castFromTextureMinificationFilter<size_t>(val)];
}

TextureMinificationFilter fromTextureMinificationGLFilter(GLenum val)
{
	return castToTextureMinificationFilter(std::find(textureMinificationFilterTable.cbegin(), textureMinificationFilterTable.cend(), val) - textureMinificationFilterTable.cbegin());
}

GLenum toTextureMagnificationGLFilter(TextureMagnificationFilter val)
{
	return textureMagnificationFilterTable[castFromTextureMagnificationFilter<size_t>(val)];
}

TextureMagnificationFilter fromTextureMagnificationGLFilter(GLenum val)
{
	return castToTextureMagnificationFilter(std::find(textureMagnificationFilterTable.cbegin(), textureMagnificationFilterTable.cend(), val) - textureMagnificationFilterTable.cbegin());
}

GLenum toTextureWrapGLMode(TextureWrapMode val)
{
	return textureWrapModeTable[castFromTextureWrapMode<size_t>(val)];
}

TextureWrapMode fromTextureWrapGLMode(GLenum val)
{
	return castToTextureWrapMode(std::find(textureWrapModeTable.cbegin(), textureWrapModeTable.cend(), val) - textureWrapModeTable.cbegin());
}

GLenum toTextureGLSwizzle(TextureSwizzle val)
{
	return textureSwizzleTable[castFromTextureSwizzle<size_t>(val)];
}

TextureSwizzle fromTextureGLSwizzle(GLenum val)
{
	return castToTextureSwizzle(std::find(textureSwizzleTable.cbegin(), textureSwizzleTable.cend(), val) - textureSwizzleTable.cbegin());
}

GLenum toTextureCompareGLFunc(TextureCompareFunc val)
{
	return textureCompareFuncTable[castFromTextureCompareFunc<size_t>(val)];
}

TextureCompareFunc fromTextureCompareGLFunc(GLenum val)
{
	return castToTextureCompareFunc(std::find(textureCompareFuncTable.cbegin(), textureCompareFuncTable.cend(), val) - textureCompareFuncTable.cbegin());
}

}
