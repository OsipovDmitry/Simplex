#include <algorithm>
#include <array>

#include <glm/glm.hpp>

#include <renderer/context.h>
#include <renderer/texture.h>
#include <types/image.h>

#include "contextprivate.h"
#include "textureprivate.h"

namespace renderer {

Texture::~Texture()
{
	m->context->m->bindThisContext();
	delete m;
}

ContextPtr Texture::context() const
{
	return m->context;
}

TextureType Texture::type() const
{
	return m->type;
}

TextureSize Texture::size() const
{
	return m->size;
}

TextureInternalFormat Texture::internalFormat() const
{
	return m->internalFormat;
}

uint32_t Texture::numLevels() const
{
	return m->numLevels;
}

void Texture::setSubImage(types::ImageFormat dataFormat, types::ImageType dataType, const void* pData, uint32_t level, const TextureSize& offset, TextureSize size)
{
	if (size.width == 0) size.width = m->size.width;
	if (size.height == 0) size.height = m->size.height;
	if (size.depth == 0) size.depth = m->size.depth;

	m->context->m->bindThisContext();
	m->context->m->bindTexture(shared_from_this());

	switch (m->type) {
		case TextureType::Type_2D: {
			CHECK_GL_ERROR(glTexSubImage2D(GL_TEXTURE_2D, level, offset.x, offset.y, size.width, size.height, toImageGLFormat(dataFormat), toImageGLType(dataType), pData), "Can not set subimage");
			break;
		}
		case TextureType::Type_3D: {
			CHECK_GL_ERROR(glTexSubImage3D(GL_TEXTURE_3D, level, offset.x, offset.y, offset.z, size.width, size.height, size.z, toImageGLFormat(dataFormat), toImageGLType(dataType), pData), "Can not set subimage");
			break;
		}
		case TextureType::Type_2DArray: {
			CHECK_GL_ERROR(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, level, offset.x, offset.y, offset.z, size.width, size.height, size.z, toImageGLFormat(dataFormat), toImageGLType(dataType), pData), "Can not set subimage");
			break;
		}
		case TextureType::Type_CubeMap: {
			LOG_ERROR("Texture::setSubImage(): Use overload method for cubemap texture");
			break;
		}
	}
}

void Texture::setSubImage(TextureCubemapSide cubemapSide, types::ImageFormat dataFormat, types::ImageType dataType, const void* pData, uint32_t level, const TextureSize& offset, TextureSize size)
{
	if (size.width == 0) size.width = m->size.width;
	if (size.height == 0) size.height = m->size.height;
	if (size.depth == 0) size.depth = m->size.depth;

	m->context->m->bindThisContext();
	m->context->m->bindTexture(shared_from_this());

	switch (m->type) {
		case TextureType::Type_2D: {
			LOG_ERROR("Texture::setSubImage(): Use overload method for 2d texture");
			break;
		}
		case TextureType::Type_3D: {
			LOG_ERROR("Texture::setSubImage(): Use overload method for 3d texture");
			break;
		}
		case TextureType::Type_2DArray: {
			LOG_ERROR("Texture::setSubImage(): Use overload method for 2d-array texture");
			break;
		}
		case TextureType::Type_CubeMap: {
			CHECK_GL_ERROR(glTexSubImage2D(toTextureCubemapGLSide(cubemapSide), level, offset.x, offset.y, size.width, size.height, toImageGLFormat(dataFormat), toImageGLType(dataType), pData), "Can not set subimage");
			break;

		}
	}
}

void Texture::generateMipmap()
{
	m->context->m->bindThisContext();
	m->context->m->bindTexture(shared_from_this());
	CHECK_GL_ERROR(glGenerateMipmap(toTextureGLType(m->type));, "Can not generate mipmap");
}

void Texture::setFilter(TextureMinificationFilter minFilter, TextureMagnificationFilter magFilter)
{
	m->context->m->bindThisContext();
	m->context->m->bindTexture(shared_from_this());
	CHECK_GL_ERROR(glTexParameteri(toTextureGLType(m->type), GL_TEXTURE_MIN_FILTER, toTextureMinificationGLFilter(minFilter)), "Can not set filter");
	CHECK_GL_ERROR(glTexParameteri(toTextureGLType(m->type), GL_TEXTURE_MAG_FILTER, toTextureMagnificationGLFilter(magFilter)), "Can not set filter");
}

TextureMinificationFilter Texture::minificationFilter() const
{
	m->context->m->bindThisContext();
	m->context->m->bindTexture(shared_from_this());
	GLint res;
	CHECK_GL_ERROR(glGetTexParameteriv(toTextureGLType(m->type), GL_TEXTURE_MIN_FILTER, &res), "Can not get min filter", TextureMinificationFilter::Nearest);
	return fromTextureMinificationGLFilter(res);
}

TextureMagnificationFilter Texture::magnificationFilter() const
{
	m->context->m->bindThisContext();
	m->context->m->bindTexture(shared_from_this());
	GLint res = GL_NEAREST;
	CHECK_GL_ERROR(glGetTexParameteriv(toTextureGLType(m->type), GL_TEXTURE_MAG_FILTER, &res), "Can not get mag filter", TextureMagnificationFilter::Nearest);
	return fromTextureMagnificationGLFilter(res);
}

void Texture::setWrapMode(TextureWrapMode sMode, TextureWrapMode tMode, TextureWrapMode rMode)
{
	m->context->m->bindThisContext();
	m->context->m->bindTexture(shared_from_this());
	CHECK_GL_ERROR(glTexParameteri(toTextureGLType(m->type), GL_TEXTURE_WRAP_S, toTextureWrapGLMode(sMode)), "Can not set wrap mode");
	CHECK_GL_ERROR(glTexParameteri(toTextureGLType(m->type), GL_TEXTURE_WRAP_T, toTextureWrapGLMode(tMode)), "Can not set wrap mode");
	CHECK_GL_ERROR(glTexParameteri(toTextureGLType(m->type), GL_TEXTURE_WRAP_R, toTextureWrapGLMode(rMode)), "Can not set wrap mode");
}

TextureWrapMode Texture::wrapModeS() const
{
	m->context->m->bindThisContext();
	m->context->m->bindTexture(shared_from_this());
	GLint res;
	CHECK_GL_ERROR(glGetTexParameteriv(toTextureGLType(m->type), GL_TEXTURE_WRAP_S, &res), "Can not get wrap mode", TextureWrapMode::Repeat);
	return fromTextureWrapGLMode(res);
}

TextureWrapMode Texture::wrapModeT() const
{
	m->context->m->bindThisContext();
	m->context->m->bindTexture(shared_from_this());
	GLint res;
	CHECK_GL_ERROR(glGetTexParameteriv(toTextureGLType(m->type), GL_TEXTURE_WRAP_T, &res), "Can not get wrap mode", TextureWrapMode::Repeat);
	return fromTextureWrapGLMode(res);
}

TextureWrapMode Texture::wrapModeR() const
{
	m->context->m->bindThisContext();
	m->context->m->bindTexture(shared_from_this());
	GLint res;
	CHECK_GL_ERROR(glGetTexParameteriv(toTextureGLType(m->type), GL_TEXTURE_WRAP_R, &res), "Can not get wrap mode", TextureWrapMode::Repeat);
	return fromTextureWrapGLMode(res);
}

void Texture::setSwizzle(TextureSwizzle rSwizzle, TextureSwizzle gSwizzle, TextureSwizzle bSwizzle, TextureSwizzle aSwizzle)
{
	m->context->m->bindThisContext();
	m->context->m->bindTexture(shared_from_this());
	CHECK_GL_ERROR(glTexParameteri(toTextureGLType(m->type), GL_TEXTURE_SWIZZLE_R, toTextureGLSwizzle(rSwizzle)), "Can not set swizzle");
	CHECK_GL_ERROR(glTexParameteri(toTextureGLType(m->type), GL_TEXTURE_SWIZZLE_G, toTextureGLSwizzle(gSwizzle)), "Can not set swizzle");
	CHECK_GL_ERROR(glTexParameteri(toTextureGLType(m->type), GL_TEXTURE_SWIZZLE_B, toTextureGLSwizzle(bSwizzle)), "Can not set swizzle");
	CHECK_GL_ERROR(glTexParameteri(toTextureGLType(m->type), GL_TEXTURE_SWIZZLE_A, toTextureGLSwizzle(aSwizzle)), "Can not set swizzle");
}

TextureSwizzle Texture::swizzleRed() const
{
	m->context->m->bindThisContext();
	m->context->m->bindTexture(shared_from_this());
	GLint res;
	CHECK_GL_ERROR(glGetTexParameteriv(toTextureGLType(m->type), GL_TEXTURE_SWIZZLE_R, &res), "Can not get swizzle", TextureSwizzle::Zero);
	return fromTextureGLSwizzle(res);
}

TextureSwizzle Texture::swizzleGreen() const
{
	m->context->m->bindThisContext();
	m->context->m->bindTexture(shared_from_this());
	GLint res;
	CHECK_GL_ERROR(glGetTexParameteriv(toTextureGLType(m->type), GL_TEXTURE_SWIZZLE_G, &res), "Can not get swizzle", TextureSwizzle::Zero);
	return fromTextureGLSwizzle(res);
}

TextureSwizzle Texture::swizzleBlue() const
{
	m->context->m->bindThisContext();
	m->context->m->bindTexture(shared_from_this());
	GLint res;
	CHECK_GL_ERROR(glGetTexParameteriv(toTextureGLType(m->type), GL_TEXTURE_SWIZZLE_B, &res), "Can not get swizzle", TextureSwizzle::Zero);
	return fromTextureGLSwizzle(res);
}

TextureSwizzle Texture::swizzleAlpha() const
{
	m->context->m->bindThisContext();
	m->context->m->bindTexture(shared_from_this());
	GLint res;
	CHECK_GL_ERROR(glGetTexParameteriv(toTextureGLType(m->type), GL_TEXTURE_SWIZZLE_A, &res), "Can not get swizzle", TextureSwizzle::Zero);
	return fromTextureGLSwizzle(res);
}

void Texture::enableCompareMode(TextureCompareFunc func)
{
	m->context->m->bindThisContext();
	m->context->m->bindTexture(shared_from_this());
	CHECK_GL_ERROR(glTexParameteri(toTextureGLType(m->type), GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE), "Can not set compare mode");
	CHECK_GL_ERROR(glTexParameteri(toTextureGLType(m->type), GL_TEXTURE_COMPARE_FUNC, toTextureCompareGLFunc(func)), "Can not set compare mode");
}

void Texture::disableCompareMode()
{
	m->context->m->bindThisContext();
	m->context->m->bindTexture(shared_from_this());
	CHECK_GL_ERROR(glTexParameteri(toTextureGLType(m->type), GL_TEXTURE_COMPARE_MODE, GL_NONE), "Can not set compare mode");
}

bool Texture::compareMode() const
{
	m->context->m->bindThisContext();
	m->context->m->bindTexture(shared_from_this());
	GLint res;
	CHECK_GL_ERROR(glGetTexParameteriv(toTextureGLType(m->type), GL_TEXTURE_COMPARE_MODE, &res), "Can not get compare mode", false);
	return res == GL_COMPARE_REF_TO_TEXTURE;
}

TextureCompareFunc Texture::compareFunc() const
{
	m->context->m->bindThisContext();
	m->context->m->bindTexture(shared_from_this());
	GLint res;
	CHECK_GL_ERROR(glGetTexParameteriv(toTextureGLType(m->type), GL_TEXTURE_COMPARE_FUNC, &res), "Can not get swizzle", TextureCompareFunc::Newer);
	return fromTextureCompareGLFunc(res);
}

Texture::Texture(ContextPtr context) :
	m(new TexturePrivate(context))
{
	m->context->m->bindThisContext();
	auto id = new GLuint(0);
	CHECK_GL_ERROR(glGenTextures(1, id), "Can not create texture");

	m->id = GLuintPtr(id, [](GLuint *p) {
		// context bind in destructor of class
		CHECK_GL_ERROR(glDeleteTextures(1, p), "Can not delete texture");
		delete p;
	});
}

Texture::Texture(ContextPtr context, TexturePtr sharedTexture) :
	m(new TexturePrivate(context))
{
	m->id = sharedTexture->m->id;
	m->internalFormat = sharedTexture->m->internalFormat;
	m->numLevels = sharedTexture->m->numLevels;
	m->size = sharedTexture->m->size;
	m->type = sharedTexture->m->type;
}

void Texture::init(TextureType type, int32_t numLevels, TextureInternalFormat internalFormat, const TextureSize& size)
{
	if (numLevels < 0)
		numLevels = glm::trunc(glm::log2(static_cast<float>(glm::max(glm::max(size.x, size.y), size.z)))) + 1;

	m->size = size;
	m->type = type;
	m->internalFormat = internalFormat;
	m->numLevels = numLevels;

	m->context->m->bindThisContext();
	m->context->m->bindTexture(shared_from_this());

	switch (type) {
		case TextureType::Type_2D: {
			CHECK_GL_ERROR(glTexStorage2D(GL_TEXTURE_2D, numLevels, toTextureInternalGLFormat(internalFormat), size.width, size.height), "Can not init texture");
			break;
		}
		case TextureType::Type_3D: {
			CHECK_GL_ERROR(glTexStorage3D(GL_TEXTURE_3D, numLevels, toTextureInternalGLFormat(internalFormat), size.width, size.height, size.depth), "Can not init texture");
			break;
		}
		case TextureType::Type_2DArray: {
			CHECK_GL_ERROR(glTexStorage3D(GL_TEXTURE_2D_ARRAY, numLevels, toTextureInternalGLFormat(internalFormat), size.width, size.height, size.depth), "Can not init texture");
			break;
		}
		case TextureType::Type_CubeMap: {
			CHECK_GL_ERROR(glTexStorage2D(GL_TEXTURE_CUBE_MAP, numLevels, toTextureInternalGLFormat(internalFormat), size.width, size.height), "Can not init texture");
			break;
		}
	}

	setFilter(TextureMinificationFilter::Nearest, TextureMagnificationFilter::Nearest);
}



}
