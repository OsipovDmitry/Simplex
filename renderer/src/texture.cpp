#include <renderer/context.h>
#include <renderer/texture.h>

#include "contextprivate.h"
#include "textureprivate.h"
#include "

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

Texture::Texture(ContextPtr context, TextureType type) :
	m(new TexturePrivate(context, nullptr, type))
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

Texture::Texture(ContextPtr context, TexturePtr sharedTexture)
{

}



}
