#include <renderer/context.h>
#include <renderer/renderbuffer.h>
#include <renderer/texture.h>

#include "context_p.h"
#include "renderbufferprivate.h"
#include "textureprivate.h"

namespace renderer {

Renderbuffer::~Renderbuffer()
{
    m->context->m()->bindThisContext();
	delete m;
}

ContextPtr Renderbuffer::context() const
{
	return m->context;
}

TextureInternalFormat Renderbuffer::internalFormat() const
{
    m->context->m()->bindThisContext();
    m->context->m()->bindRenderbuffer(shared_from_this());
	GLint value;
	CHECK_GL_ERROR(glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, &value), "Can not get renderbuffer internal format", TextureInternalFormat::Count);
	return fromTextureInternalGLFormat(value);
}

uint32_t Renderbuffer::width()
{
    m->context->m()->bindThisContext();
    m->context->m()->bindRenderbuffer(shared_from_this());
	GLint value = -1;
	CHECK_GL_ERROR(glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &value), "Can not get renderbuffer width", value);
	return value;
}

uint32_t Renderbuffer::height()
{
    m->context->m()->bindThisContext();
    m->context->m()->bindRenderbuffer(shared_from_this());
	GLint value = -1;
	CHECK_GL_ERROR(glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &value), "Can not get renderbuffer height", value);
	return value;
}

Renderbuffer::Renderbuffer(ContextPtr context) :
	m(new RenderbufferPrivate(context, nullptr))
{
    m->context->m()->bindThisContext();
	auto id = new GLuint(0);
	CHECK_GL_ERROR(glGenRenderbuffers(1, id), "Can not create renderbuffer");

	m->id = GLuintPtr(id, [](GLuint *p) {
		// context bind in destructor of class
		CHECK_GL_ERROR(glDeleteRenderbuffers(1, p), "Can not delete renderbuffer");
		delete p;
	});
}

void Renderbuffer::init(TextureInternalFormat internalFormat, uint32_t width, uint32_t height)
{
    m->context->m()->bindThisContext();
    m->context->m()->bindRenderbuffer(shared_from_this());
	CHECK_GL_ERROR(glRenderbufferStorage(GL_RENDERBUFFER, toTextureInternalGLFormat(internalFormat), width, height), "Can not init renderbuffer");
}

}
