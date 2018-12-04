#include <renderer/context.h>
#include <renderer/renderbuffer.h>
#include <renderer/texture.h>

#include "glutils.h"
#include "context_p.h"
#include "renderbuffer_p.h"
#include "texture_p.h"

namespace renderer {

ContextPtr Renderbuffer::context() const
{
	return m_->context;
}

TextureInternalFormat Renderbuffer::internalFormat() const
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindRenderbuffer(this);
	GLint value;
	CHECK_GL_ERROR(glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, &value), "Can not get renderbuffer internal format", TextureInternalFormat::Count);
	return fromTextureInternalGLFormat(value);
}

uint32_t Renderbuffer::width()
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindRenderbuffer(this);
	GLint value = -1;
	CHECK_GL_ERROR(glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &value), "Can not get renderbuffer width", value);
	return value;
}

uint32_t Renderbuffer::height()
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindRenderbuffer(this);
	GLint value = -1;
	CHECK_GL_ERROR(glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &value), "Can not get renderbuffer height", value);
    return value;
}

RenderbufferPtr Renderbuffer::create(ContextPtr context, TextureInternalFormat internalFormat, uint32_t width, uint32_t height)
{
    return RenderbufferPtr(new Renderbuffer(context, internalFormat, width, height), RenderbufferDeleter());
}

Renderbuffer::Renderbuffer(ContextPtr context, TextureInternalFormat internalFormat, uint32_t width, uint32_t height) :
    m_(new RenderbufferPrivate(context))
{
    m_->context->m()->bindThisContext();
    CHECK_GL_ERROR(glGenRenderbuffers(1, &m_->id), "Can not create renderbuffer");
    m_->context->m()->bindRenderbuffer(this);
    CHECK_GL_ERROR(glRenderbufferStorage(GL_RENDERBUFFER, toTextureInternalGLFormat(internalFormat), width, height), "Can not init renderbuffer");
}

Renderbuffer::~Renderbuffer()
{
    m_->context->m()->bindThisContext();
    CHECK_GL_ERROR(glDeleteRenderbuffers(1, &m_->id), "Can not delete renderbuffer");
    delete m_;
}

}
