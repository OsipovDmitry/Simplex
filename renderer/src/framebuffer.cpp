#include <glm/gtc/type_ptr.hpp>

#include <types/geometry.h>

#include <renderer/context.h>
#include <renderer/framebuffer.h>
#include <renderer/renderbuffer.h>
#include <renderer/texture.h>

#include "glutils.h"
#include "context_p.h"
#include "renderbuffer_p.h"
#include "framebuffer_p.h"
#include "texture_p.h"

namespace renderer {

ContextPtr Framebuffer::context() const
{
    return m_->context;
}

void Framebuffer::attachBuffer(FramebufferAttachment attachment, RenderbufferPtr renderbuffer)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindFramebuffer(this);
    CHECK_GL_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, toFramebufferGLAttachment(attachment), GL_RENDERBUFFER, renderbuffer ? renderbuffer->m()->id : 0), "Can not attach renderbuffer");

    m_->renderbuffers[castFromFramebufferAttachment(attachment)] = renderbuffer;
    m_->textures[castFromFramebufferAttachment(attachment)].reset();
}

void Framebuffer::attachBuffer(FramebufferAttachment attachment, TexturePtr texture, uint32_t level)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindFramebuffer(this);

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

    m_->renderbuffers[castFromFramebufferAttachment(attachment)].reset();
    m_->textures[castFromFramebufferAttachment(attachment)] = texture;
}

void Framebuffer::attachBuffer(FramebufferAttachment attachment, TexturePtr texture, TextureCubemapSide side, uint32_t level)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindFramebuffer(this);

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

    m_->renderbuffers[castFromFramebufferAttachment(attachment)].reset();
    m_->textures[castFromFramebufferAttachment(attachment)] = texture;
}

void Framebuffer::attachBuffer(FramebufferAttachment attachment, TexturePtr texture, uint32_t layer, uint32_t level)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindFramebuffer(this);

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

    m_->renderbuffers[castFromFramebufferAttachment(attachment)].reset();
    m_->textures[castFromFramebufferAttachment(attachment)] = texture;
}

void Framebuffer::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    m_->viewport = glm::uvec4(x, y, width, height);

    m_->context->m()->bindThisContext();
    m_->context->m()->bindFramebuffer(this);
    CHECK_GL_ERROR(glViewport(m_->viewport.x, m_->viewport.y, m_->viewport.z, m_->viewport.w), "Can not set viewport");
}

glm::uvec4 Framebuffer::viewport() const
{
    return m_->viewport;
}

void Framebuffer::clearColorBuffer(uint32_t index, const glm::vec4& value)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindFramebuffer(this);
	CHECK_GL_ERROR(glClearBufferfv(GL_COLOR, index, glm::value_ptr(value)), "Can not clear framebuffer");
}

void Framebuffer::clearColorBuffer(uint32_t index, const glm::uvec4& value)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindFramebuffer(this);
	CHECK_GL_ERROR(glClearBufferuiv(GL_COLOR, index, glm::value_ptr(value)), "Can not clear framebuffer");
}

void Framebuffer::clearColorBuffer(uint32_t index, const glm::ivec4& value)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindFramebuffer(this);
	CHECK_GL_ERROR(glClearBufferiv(GL_COLOR, index, glm::value_ptr(value)), "Can not clear framebuffer");
}

void Framebuffer::clearDepthBuffer(float value)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindFramebuffer(this);
    CHECK_GL_ERROR(glClearBufferfv(GL_DEPTH, 0, &value), "Can not clear framebuffer");
}

void Framebuffer::clearStencilBuffer(int32_t value)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindFramebuffer(this);
	CHECK_GL_ERROR(glClearBufferiv(GL_STENCIL, 0, &value), "Can not clear framebuffer");
}

void Framebuffer::clearDepthStencilBuffer(float depth, int32_t stencil)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindFramebuffer(this);
	CHECK_GL_ERROR(glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil), "Can not clear framebuffer");
}

void Framebuffer::copyColorBuffer(const glm::uvec4& dstRect, const glm::uvec4& srcRect, FramebufferPtr srcBuffer, uint32_t dstColorIndex, uint32_t srcColorIndex)
{
    GLenum dstColorBuffer = (m_->id == 0) ? GL_BACK : GL_COLOR_ATTACHMENT0 + dstColorIndex;
    GLenum srcColorBuffer = (srcBuffer->m_->id == 0) ? GL_BACK : GL_COLOR_ATTACHMENT0 + srcColorIndex;

    m_->context->m()->bindThisContext();
    CHECK_GL_ERROR(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_->id), "Can not copy framebuffer");
	CHECK_GL_ERROR(glDrawBuffers(1, &dstColorBuffer), "Can not copy framebuffer");
    CHECK_GL_ERROR(glBindFramebuffer(GL_READ_FRAMEBUFFER, srcBuffer->m_->id), "Can not copy framebuffer");
	CHECK_GL_ERROR(glReadBuffer(srcColorBuffer), "Can not copy framebuffer");
	CHECK_GL_ERROR(glBlitFramebuffer(srcRect.x, srcRect.y, srcRect.z, srcRect.w, dstRect.x, dstRect.y, dstRect.z, dstRect.w, GL_COLOR_BUFFER_BIT, GL_LINEAR), "Can not copy framebuffer");
}

void Framebuffer::copyDepthBuffer(const glm::uvec4& dstRect, const glm::uvec4& srcRect, FramebufferPtr srcBuffer)
{
    m_->context->m()->bindThisContext();
    CHECK_GL_ERROR(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_->id), "Can not copy framebuffer");
    CHECK_GL_ERROR(glBindFramebuffer(GL_READ_FRAMEBUFFER, srcBuffer->m_->id), "Can not copy framebuffer");
	CHECK_GL_ERROR(glBlitFramebuffer(srcRect.x, srcRect.y, srcRect.z, srcRect.w, dstRect.x, dstRect.y, dstRect.z, dstRect.w, GL_DEPTH_BUFFER_BIT, GL_NEAREST), "Can not copy framebuffer");
}

void Framebuffer::copyStencilBuffer(const glm::uvec4& dstRect, const glm::uvec4& srcRect, FramebufferPtr srcBuffer)
{
    m_->context->m()->bindThisContext();
    CHECK_GL_ERROR(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_->id), "Can not copy framebuffer");
    CHECK_GL_ERROR(glBindFramebuffer(GL_READ_FRAMEBUFFER, srcBuffer->m_->id), "Can not copy framebuffer");
	CHECK_GL_ERROR(glBlitFramebuffer(srcRect.x, srcRect.y, srcRect.z, srcRect.w, dstRect.x, dstRect.y, dstRect.z, dstRect.w, GL_STENCIL_BUFFER_BIT, GL_NEAREST), "Can not copy framebuffer");
}

void Framebuffer::renderIndexedGeometry(ProgramPtr program, VertexArrayPtr vertexArray, types::PrimitiveType primitiveType, uint32_t numIndices, types::IndexType indicesType, const void* pIndices)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindFramebuffer(this);
    m_->context->m()->bindProgram(program.get());
    m_->context->m()->bindVertexArray(vertexArray.get());

	CHECK_GL_ERROR(glDrawElements(toPrimitiveGLType(primitiveType), numIndices, toGeometryIndexGLType(indicesType), pIndices), "Can not render indexed geometry");
}

void Framebuffer::renderIndexedGeometry(ProgramPtr program, VertexArrayPtr vertexArray, types::PrimitiveType primitiveType, uint32_t numIndices, types::IndexType indicesType, uint32_t bufferOffset)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindFramebuffer(this);
    m_->context->m()->bindProgram(program.get());
    m_->context->m()->bindVertexArray(vertexArray.get());

    CHECK_GL_ERROR(glDrawElements(toPrimitiveGLType(primitiveType), numIndices, toGeometryIndexGLType(indicesType), (const void*)bufferOffset), "Can not render indexed geometry");
}

FramebufferPtr Framebuffer::create(ContextPtr context)
{
    return FramebufferPtr(new Framebuffer(context, std::false_type()), FramebufferDeleter());
}

FramebufferPtr Framebuffer::mainFramebuffer(ContextPtr context)
{
    return FramebufferPtr(new Framebuffer(context, std::true_type()), FramebufferDeleter());
}

Framebuffer::Framebuffer(ContextPtr context, std::false_type) :
    m_(new FramebufferPrivate(context))
{
    m_->context->m()->bindThisContext();
    CHECK_GL_ERROR(glGenFramebuffers(1, &m_->id), "Can not create framebuffer");
}

Framebuffer::Framebuffer(ContextPtr context, std::true_type) :
    m_(new FramebufferPrivate(context))
{
    m_->id = 0;
}

Framebuffer::~Framebuffer()
{
    m_->context->m()->bindThisContext();
    if (m_->id != 0)
        CHECK_GL_ERROR(glDeleteFramebuffers(1, &m_->id), "Can not delete framebuffer");
    delete m_;
}


}
