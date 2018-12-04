#include <glm/gtc/type_ptr.hpp>

#include <renderer/context.h>
#include <renderer/framebuffer.h>
#include <renderer/renderbuffer.h>
#include <renderer/texture.h>

#include "context_p.h"
#include "renderbufferprivate.h"
#include "framebufferprivate.h"
#include "texture_p.h"

namespace renderer {

Framebuffer::~Framebuffer()
{
    m->context->m()->bindThisContext();
	delete m;
}

ContextPtr Framebuffer::context() const
{
	return m->context;
}

void Framebuffer::attachBuffer(FramebufferAttachment attachment, RenderbufferPtr renderbuffer)
{
    m->context->m()->bindThisContext();
    m->context->m()->bindFramebuffer(shared_from_this());
	CHECK_GL_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, toFramebufferGLAttachment(attachment), GL_RENDERBUFFER, renderbuffer ? *renderbuffer->m->id : 0), "Can not attach renderbuffer");

	m->renderbuffers[castFromFramebufferAttachment<size_t>(attachment)] = renderbuffer;
	m->textures[castFromFramebufferAttachment<size_t>(attachment)].reset();
}

void Framebuffer::attachBuffer(FramebufferAttachment attachment, TexturePtr texture, uint32_t level)
{
    m->context->m()->bindThisContext();
    m->context->m()->bindFramebuffer(shared_from_this());

    switch (texture->m()->type) {
		case TextureType::Type_2D: {
            CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, toFramebufferGLAttachment(attachment), GL_TEXTURE_2D, texture ? texture->m()->id : 0, level), "Can not attach texture");
			break;
		}
		case TextureType::Type_3D: {
			LOG_ERROR("Framebuffer::attachBuffer(): Use overload method for 3d texture");
			break;
		}
		case TextureType::Type_2DArray: {
			LOG_ERROR("Framebuffer::attachBuffer(): Use overload method for 2d-array texture");
			break;
		}
		case TextureType::Type_CubeMap: {
			LOG_ERROR("Framebuffer::attachBuffer(): Use overload method for cubemap texture");
			break;
		}
	}

	m->renderbuffers[castFromFramebufferAttachment<size_t>(attachment)].reset();
	m->textures[castFromFramebufferAttachment<size_t>(attachment)] = texture;
}

void Framebuffer::attachBuffer(FramebufferAttachment attachment, TexturePtr texture, TextureCubemapSide side, uint32_t level)
{
    m->context->m()->bindThisContext();
    m->context->m()->bindFramebuffer(shared_from_this());

    switch (texture->m()->type) {
		case TextureType::Type_2D: {
			LOG_ERROR("Framebuffer::attachBuffer(): Use overload method for 2d texture");
			break;
		}
		case TextureType::Type_3D: {
			LOG_ERROR("Framebuffer::attachBuffer(): Use overload method for 3d texture");
			break;
		}
		case TextureType::Type_2DArray: {
			LOG_ERROR("Framebuffer::attachBuffer(): Use overload method for 2d-array texture");
			break;
		}
		case TextureType::Type_CubeMap: {
            CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, toFramebufferGLAttachment(attachment), toTextureCubemapGLSide(side), texture ? texture->m()->id : 0, level), "Can not attach texture");
			break;
		}
	}

	m->renderbuffers[castFromFramebufferAttachment<size_t>(attachment)].reset();
	m->textures[castFromFramebufferAttachment<size_t>(attachment)] = texture;
}

void Framebuffer::attachBuffer(FramebufferAttachment attachment, TexturePtr texture, uint32_t layer, uint32_t level)
{
    m->context->m()->bindThisContext();
    m->context->m()->bindFramebuffer(shared_from_this());

    switch (texture->m()->type) {
		case TextureType::Type_2D: {
			LOG_ERROR("Framebuffer::attachBuffer(): Use overload method for 2d texture");
			break;
		}
		case TextureType::Type_3D:
		case TextureType::Type_2DArray: {
            CHECK_GL_ERROR(glFramebufferTextureLayer(GL_FRAMEBUFFER, toFramebufferGLAttachment(attachment), texture ? texture->m()->id : 0, level, layer), "Can not attach texture");
			break;
		}
		case TextureType::Type_CubeMap: {
			LOG_ERROR("Framebuffer::attachBuffer(): Use overload method for cubemap texture");
			break;
		}
	}

	m->renderbuffers[castFromFramebufferAttachment<size_t>(attachment)].reset();
	m->textures[castFromFramebufferAttachment<size_t>(attachment)] = texture;
}

void Framebuffer::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	m->viewport = glm::uvec4(x, y, width, height);

    m->context->m()->bindThisContext();
    m->context->m()->bindFramebuffer(shared_from_this());
	CHECK_GL_ERROR(glViewport(m->viewport.x, m->viewport.y, m->viewport.z, m->viewport.w), "Can not set viewport");
}

glm::uvec4 Framebuffer::viewport() const
{
	return m->viewport;
}

void Framebuffer::clearColorBuffer(uint32_t index, const glm::vec4& value)
{
    m->context->m()->bindThisContext();
    m->context->m()->bindFramebuffer(shared_from_this());
	CHECK_GL_ERROR(glClearBufferfv(GL_COLOR, index, glm::value_ptr(value)), "Can not clear framebuffer");
}

void Framebuffer::clearColorBuffer(uint32_t index, const glm::uvec4& value)
{
    m->context->m()->bindThisContext();
    m->context->m()->bindFramebuffer(shared_from_this());
	CHECK_GL_ERROR(glClearBufferuiv(GL_COLOR, index, glm::value_ptr(value)), "Can not clear framebuffer");
}

void Framebuffer::clearColorBuffer(uint32_t index, const glm::ivec4& value)
{
    m->context->m()->bindThisContext();
    m->context->m()->bindFramebuffer(shared_from_this());
	CHECK_GL_ERROR(glClearBufferiv(GL_COLOR, index, glm::value_ptr(value)), "Can not clear framebuffer");
}

void Framebuffer::clearDepthBuffer(float value)
{
    m->context->m()->bindThisContext();
    m->context->m()->bindFramebuffer(shared_from_this());
	CHECK_GL_ERROR(glClearBufferfv(GL_DEPTH, 0, &value), "Can not clear framebuffer");
}

void Framebuffer::clearStencilBuffer(int32_t value)
{
    m->context->m()->bindThisContext();
    m->context->m()->bindFramebuffer(shared_from_this());
	CHECK_GL_ERROR(glClearBufferiv(GL_STENCIL, 0, &value), "Can not clear framebuffer");
}

void Framebuffer::clearDepthStencilBuffer(float depth, int32_t stencil)
{
    m->context->m()->bindThisContext();
    m->context->m()->bindFramebuffer(shared_from_this());
	CHECK_GL_ERROR(glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil), "Can not clear framebuffer");
}

void Framebuffer::copyColorBuffer(const glm::uvec4& dstRect, const glm::uvec4& srcRect, FramebufferPtr srcBuffer, uint32_t dstColorIndex, uint32_t srcColorIndex)
{
	GLenum dstColorBuffer = (*m->id == 0) ? GL_BACK : GL_COLOR_ATTACHMENT0 + dstColorIndex;
	GLenum srcColorBuffer = (*srcBuffer->m->id == 0) ? GL_BACK : GL_COLOR_ATTACHMENT0 + srcColorIndex;

    m->context->m()->bindThisContext();
	CHECK_GL_ERROR(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *m->id), "Can not copy framebuffer");
	CHECK_GL_ERROR(glDrawBuffers(1, &dstColorBuffer), "Can not copy framebuffer");
	CHECK_GL_ERROR(glBindFramebuffer(GL_READ_FRAMEBUFFER, *srcBuffer->m->id), "Can not copy framebuffer");
	CHECK_GL_ERROR(glReadBuffer(srcColorBuffer), "Can not copy framebuffer");
	CHECK_GL_ERROR(glBlitFramebuffer(srcRect.x, srcRect.y, srcRect.z, srcRect.w, dstRect.x, dstRect.y, dstRect.z, dstRect.w, GL_COLOR_BUFFER_BIT, GL_LINEAR), "Can not copy framebuffer");
}

void Framebuffer::copyDepthBuffer(const glm::uvec4& dstRect, const glm::uvec4& srcRect, FramebufferPtr srcBuffer)
{
    m->context->m()->bindThisContext();
	CHECK_GL_ERROR(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *m->id), "Can not copy framebuffer");
	CHECK_GL_ERROR(glBindFramebuffer(GL_READ_FRAMEBUFFER, *srcBuffer->m->id), "Can not copy framebuffer");
	CHECK_GL_ERROR(glBlitFramebuffer(srcRect.x, srcRect.y, srcRect.z, srcRect.w, dstRect.x, dstRect.y, dstRect.z, dstRect.w, GL_DEPTH_BUFFER_BIT, GL_NEAREST), "Can not copy framebuffer");
}

void Framebuffer::copyStencilBuffer(const glm::uvec4& dstRect, const glm::uvec4& srcRect, FramebufferPtr srcBuffer)
{
    m->context->m()->bindThisContext();
	CHECK_GL_ERROR(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *m->id), "Can not copy framebuffer");
	CHECK_GL_ERROR(glBindFramebuffer(GL_READ_FRAMEBUFFER, *srcBuffer->m->id), "Can not copy framebuffer");
	CHECK_GL_ERROR(glBlitFramebuffer(srcRect.x, srcRect.y, srcRect.z, srcRect.w, dstRect.x, dstRect.y, dstRect.z, dstRect.w, GL_STENCIL_BUFFER_BIT, GL_NEAREST), "Can not copy framebuffer");
}

void Framebuffer::renderIndexedGeometry(ProgramPtr program, VertexArrayPtr vertexArray, PrimitiveType primitiveType, uint32_t numIndices, GeometryIndexType indicesType, const void* pIndices)
{
    m->context->m()->bindThisContext();
    m->context->m()->bindFramebuffer(shared_from_this());
    m->context->m()->bindProgram(program.get());
    m->context->m()->bindVertexArray(vertexArray);

	CHECK_GL_ERROR(glDrawElements(toPrimitiveGLType(primitiveType), numIndices, toGeometryIndexGLType(indicesType), pIndices), "Can not render indexed geometry");
}

void Framebuffer::renderIndexedGeometry(ProgramPtr program, VertexArrayPtr vertexArray, PrimitiveType primitiveType, uint32_t numIndices, GeometryIndexType indicesType, uint32_t bufferOffset)
{
    m->context->m()->bindThisContext();
    m->context->m()->bindFramebuffer(shared_from_this());
    m->context->m()->bindProgram(program.get());
    m->context->m()->bindVertexArray(vertexArray);

	CHECK_GL_ERROR(glDrawElements(toPrimitiveGLType(primitiveType), numIndices, toGeometryIndexGLType(indicesType), (const void*)bufferOffset), "Can not render indexed geometry");
}


Framebuffer::Framebuffer(ContextPtr context, std::false_type) :
	m(new FramebufferPrivate(context, nullptr))
{
    m->context->m()->bindThisContext();
	auto id = new GLuint(0);
	CHECK_GL_ERROR(glGenFramebuffers(1, id), "Can not create framebuffer");

	m->id = GLuintPtr(id, [](GLuint *p) {
		// context bind in destructor of class
		CHECK_GL_ERROR(glDeleteFramebuffers(1, p), "Can not delete framebuffer");
		delete p;
});
}

Framebuffer::Framebuffer(ContextPtr context, std::true_type) :
	m(new FramebufferPrivate(context, std::make_shared<GLuint>(0)))
{
}


}
