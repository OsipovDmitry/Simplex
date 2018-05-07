#include <algorithm>

#include "textureprivate.h"

namespace renderer {

const std::array<GLenum, castFromTextureType<size_t>(TextureType::Count)> textureTypeTable {
	GL_TEXTURE_2D,
	GL_TEXTURE_3D,
	GL_TEXTURE_2D_ARRAY,
	GL_TEXTURE_CUBE_MAP
};

GLenum toTextureGLType(TextureType val)
{
	return textureTypeTable[castFromTextureType<size_t>(val)];
}

TextureType fromTextureGLType(GLenum val)
{
	return castToTextureType(std::find(textureTypeTable.cbegin(), textureTypeTable.cend(), val) - textureTypeTable.cbegin());
}

const std::array<GLenum, castFromTextureInternalFormat<size_t>(TextureInternalFormat::Count)> textureInternalFormatTable {
	GL_R8, GL_R8_SNORM, GL_R16F, GL_R32F, GL_R8UI, GL_R8I, GL_R16UI, GL_R16I, GL_R32UI, GL_R32I,
	GL_RG8, GL_RG8_SNORM, GL_RG16F, GL_RG32F, GL_RG8UI, GL_RG8I, GL_RG16UI, GL_RG16I, GL_RG32UI, GL_RG32I,
	GL_RGB8, GL_RGB8_SNORM, GL_RGB16F, GL_RGB32F, GL_RGB8UI, GL_RGB8I, GL_RGB16UI, GL_RGB16I, GL_RGB32UI, GL_RGB32I,
	GL_RGBA8, GL_RGBA8_SNORM, GL_RGBA16F, GL_RGBA32F, GL_RGBA8UI, GL_RGBA8I, GL_RGBA16UI, GL_RGBA16I, GL_RGBA32UI, GL_RGBA32I,
	GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT32F, GL_DEPTH24_STENCIL8,
};

GLenum toTextureInternalGLFormat(TextureInternalFormat val)
{
	return textureInternalFormatTable[castFromTextureInternalFormat<size_t>(val)];
}

TextureInternalFormat fromTextureInternalGLFormat(GLenum val)
{
	return castToTextureInternalFormat(std::find(textureInternalFormatTable.cbegin(), textureInternalFormatTable.cend(), val) - textureInternalFormatTable.cbegin());
}

const std::array<GLenum, types::castFromImageFormat<size_t>(types::ImageFormat::Count)> imageFormatTable {
	GL_RED,
	GL_RG,
	GL_RGB,
	GL_RGBA,
	GL_DEPTH_COMPONENT,
	GL_DEPTH_STENCIL,
	GL_LUMINANCE,
	GL_LUMINANCE_ALPHA,
	GL_ALPHA
};

GLenum toImageGLFormat(types::ImageFormat val)
{
	return imageFormatTable[types::castFromImageFormat<size_t>(val)];
}

types::ImageFormat fromImageGLFormat(GLenum val)
{
	return types::castToImageFormat(std::find(imageFormatTable.cbegin(), imageFormatTable.cend(), val) - imageFormatTable.cbegin());
}

const std::array<GLenum, types::castFromImageType<size_t>(types::ImageType::Count)> imageTypeTable {
	GL_UNSIGNED_BYTE, GL_BYTE,
	GL_UNSIGNED_SHORT, GL_SHORT,
	GL_UNSIGNED_INT, GL_INT,
	GL_HALF_FLOAT, GL_FLOAT
};

GLenum toImageGLType(types::ImageType val)
{
	return imageTypeTable[types::castFromImageType<size_t>(val)];
}

types::ImageType fromImageGLType(GLenum val)
{
	return types::castToImageType(std::find(imageTypeTable.cbegin(), imageTypeTable.cend(), val) - imageTypeTable.cbegin());
}

const std::array<GLenum, castFromTextureCubemapSide<size_t>(TextureCubemapSide::Count)> textureCubemapSideTable {
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
};

GLenum toTextureCubemapGLSide(TextureCubemapSide val)
{
	return textureCubemapSideTable[castFromTextureCubemapSide<size_t>(val)];
}

TextureCubemapSide fromTextureCubemapGLSide(GLenum val)
{
	return castToTextureCubemapSide(std::find(textureCubemapSideTable.cbegin(), textureCubemapSideTable.cend(), val) - textureCubemapSideTable.cbegin());
}

const std::array<GLenum, castFromTextureMinificationFilter<size_t>(TextureMinificationFilter::Count)> textureMinificationFilterTable {
	GL_NEAREST,
	GL_LINEAR,
	GL_NEAREST_MIPMAP_NEAREST,
	GL_NEAREST_MIPMAP_LINEAR,
	GL_LINEAR_MIPMAP_NEAREST,
	GL_LINEAR_MIPMAP_LINEAR
};

GLenum toTextureMinificationGLFilter(TextureMinificationFilter val)
{
	return textureMinificationFilterTable[castFromTextureMinificationFilter<size_t>(val)];
}

TextureMinificationFilter fromTextureMinificationGLFilter(GLenum val)
{
	return castToTextureMinificationFilter(std::find(textureMinificationFilterTable.cbegin(), textureMinificationFilterTable.cend(), val) - textureMinificationFilterTable.cbegin());
}

const std::array<GLenum, castFromTextureMagnificationFilter<size_t>(TextureMagnificationFilter::Count)> textureMagnificationFilterTable {
	GL_NEAREST,
	GL_LINEAR
};

GLenum toTextureMagnificationGLFilter(TextureMagnificationFilter val)
{
	return textureMagnificationFilterTable[castFromTextureMagnificationFilter<size_t>(val)];
}

TextureMagnificationFilter fromTextureMagnificationGLFilter(GLenum val)
{
	return castToTextureMagnificationFilter(std::find(textureMagnificationFilterTable.cbegin(), textureMagnificationFilterTable.cend(), val) - textureMagnificationFilterTable.cbegin());
}

const std::array<GLenum, castFromTextureWrapMode<size_t>(TextureWrapMode::Count)> textureWrapModeTable {
	GL_CLAMP_TO_EDGE,
	GL_REPEAT,
	GL_MIRRORED_REPEAT
};

GLenum toTextureWrapGLMode(TextureWrapMode val)
{
	return textureWrapModeTable[castFromTextureWrapMode<size_t>(val)];
}

TextureWrapMode fromTextureWrapGLMode(GLenum val)
{
	return castToTextureWrapMode(std::find(textureWrapModeTable.cbegin(), textureWrapModeTable.cend(), val) - textureWrapModeTable.cbegin());
}

const std::array<GLenum, castFromTextureSwizzle<size_t>(TextureSwizzle::Count)> textureSwizzleTable {
	GL_RED,
	GL_GREEN,
	GL_BLUE,
	GL_ALPHA,
	GL_ZERO,
	GL_ONE
};

GLenum toTextureGLSwizzle(TextureSwizzle val)
{
	return textureSwizzleTable[castFromTextureSwizzle<size_t>(val)];
}

TextureSwizzle fromTextureGLSwizzle(GLenum val)
{
	return castToTextureSwizzle(std::find(textureSwizzleTable.cbegin(), textureSwizzleTable.cend(), val) - textureSwizzleTable.cbegin());
}

const std::array<GLenum, castFromTextureCompareFunc<size_t>(TextureCompareFunc::Count)> textureCompareFuncTable {
	GL_LEQUAL,
	GL_GEQUAL,
	GL_LESS,
	GL_GREATER,
	GL_EQUAL,
	GL_NOTEQUAL,
	GL_ALWAYS,
	GL_NEVER
};

GLenum toTextureCompareGLFunc(TextureCompareFunc val)
{
	return textureCompareFuncTable[castFromTextureCompareFunc<size_t>(val)];
}

TextureCompareFunc fromTextureCompareGLFunc(GLenum val)
{
	return castToTextureCompareFunc(std::find(textureCompareFuncTable.cbegin(), textureCompareFuncTable.cend(), val) - textureCompareFuncTable.cbegin());
}

}
