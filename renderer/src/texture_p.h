#ifndef TEXTUREPRIVATE_H
#define TEXTUREPRIVATE_H

#include <GLES3/gl3.h>

#include <renderer/texture.h>
#include <types/image.h>

namespace renderer {

GLenum toTextureGLType(TextureType val);
TextureType fromTextureGLType(GLenum val);

GLenum toTextureInternalGLFormat(TextureInternalFormat val);
TextureInternalFormat fromTextureInternalGLFormat(GLenum val);

GLenum toImageGLFormat(types::ImageFormat val);
types::ImageFormat fromImageGLFormat(GLenum val);

GLenum toImageGLType(types::ImageType val);
types::ImageType fromImageGLType(GLenum val);

GLenum toTextureCubemapGLSide(TextureCubemapSide val);
TextureCubemapSide fromTextureCubemapGLSide(GLenum val);

GLenum toTextureMinificationGLFilter(TextureMinificationFilter val);
TextureMinificationFilter fromTextureMinificationGLFilter(GLenum val);

GLenum toTextureMagnificationGLFilter(TextureMagnificationFilter val);
TextureMagnificationFilter fromTextureMagnificationGLFilter(GLenum val);

GLenum toTextureWrapGLMode(TextureWrapMode val);
TextureWrapMode fromTextureWrapGLMode(GLenum val);

GLenum toTextureGLSwizzle(TextureSwizzle val);
TextureSwizzle fromTextureGLSwizzle(GLenum val);

GLenum toTextureCompareGLFunc(TextureCompareFunc val);
TextureCompareFunc fromTextureCompareGLFunc(GLenum val);

class TexturePrivate {
public:
	TextureSize size;
	TextureType type;
	ContextPtr context;
    GLuint id;
	TextureInternalFormat internalFormat;
	uint32_t numLevels;

	TexturePrivate(ContextPtr context_) :
		size(),
		type(),
		context(context_),
        id(0),
		internalFormat()
	{}
};

}

#endif // TEXTUREPRIVATE_H
