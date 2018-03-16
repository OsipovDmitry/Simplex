#ifndef TEXTUREPRIVATE_H
#define TEXTUREPRIVATE_H

#include <renderer/texture.h>
#include <types/image.h>

#include "glutils.h"

namespace renderer {

const std::array<GLenum, castFromTextureType<size_t>(TextureType::Count)> textureTypeTable {
	GL_TEXTURE_2D,
	GL_TEXTURE_3D,
	GL_TEXTURE_2D_ARRAY,
	GL_TEXTURE_CUBE_MAP
};
GLenum toTextureGLType(TextureType val);
TextureType fromShaderGLType(GLenum val);

const std::array<GLenum, castFromTextureInternalFormat<size_t>(TextureInternalFormat::Count)> textureInternalFormatTable {
	GL_R8, GL_R8_SNORM, GL_R16F, GL_R32F, GL_R8UI, GL_R8I, GL_R16UI, GL_R16I, GL_R32UI, GL_R32I,
	GL_RG8, GL_RG8_SNORM, GL_RG16F, GL_RG32F, GL_RG8UI, GL_RG8I, GL_RG16UI, GL_RG16I, GL_RG32UI, GL_RG32I,
	GL_RGB8, GL_RGB8_SNORM, GL_RGB16F, GL_RGB32F, GL_RGB8UI, GL_RGB8I, GL_RGB16UI, GL_RGB16I, GL_RGB32UI, GL_RGB32I,
	GL_RGBA8, GL_RGBA8_SNORM, GL_RGBA16F, GL_RGBA32F, GL_RGBA8UI, GL_RGBA8I, GL_RGBA16UI, GL_RGBA16I, GL_RGBA32UI, GL_RGBA32I,
	GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT32F, GL_DEPTH24_STENCIL8,
};
GLenum toTextureInternalGLFormat(TextureInternalFormat val);
TextureInternalFormat fromTextureInternalGLFormat(GLenum val);

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
GLenum toImageGLFormat(types::ImageFormat val);
types::ImageFormat fromImageGLFormat(GLenum val);

const std::array<GLenum, types::castFromImageType<size_t>(types::ImageType::Count)> imageTypeTable {
	GL_UNSIGNED_BYTE, GL_BYTE,
	GL_UNSIGNED_SHORT, GL_SHORT,
	GL_UNSIGNED_INT, GL_INT,
	GL_HALF_FLOAT, GL_FLOAT
};
GLenum toImageGLType(types::ImageType val);
types::ImageType fromImageGLType(GLenum val);

const std::array<GLenum, castFromTextureCubemapSide<size_t>(TextureCubemapSide::Count)> textureCubemapSideTable {
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
};
GLenum toTextureCubemapGLSide(TextureCubemapSide val);
TextureCubemapSide fromTextureCubemapGLSide(GLenum val);

class TexturePrivate {
public:
	TextureSize size;
	TextureType type;
	ContextPtr context;
	GLuintPtr id;
	TextureInternalFormat internalFormat;
	uint32_t numLevels;

	TexturePrivate(ContextPtr context_) :
		size(),
		type(),
		context(context_),
		id(),
		internalFormat()
	{}
};

}

#endif // TEXTUREPRIVATE_H
