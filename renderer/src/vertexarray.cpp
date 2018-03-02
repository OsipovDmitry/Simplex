#include <renderer/context.h>
#include <renderer/vertexarray.h>

#include "contextprivate.h"
#include "vertexarrayprivate.h"
#include "checkglerror.h"

namespace renderer {

VertexArray::~VertexArray()
{
	auto context = m->context;
	auto id = m->id;
	delete m;

	context->m->bindThisContext();
	CHECK_GL_ERROR(glDeleteVertexArrays(1, &id), "Can not delete VAO");
}

void VertexArray::bindIndexBuffer(BufferPtr buffer)
{
	m->context->m->bindThisContext();
	m->context->m->bindVertexArray(shared_from_this());
	m->context->m->bindBuffer(BufferTarget::Element, buffer);
	m->context->m->bindVertexArray(nullptr);
}

VertexArray::VertexArray(ContextPtr context) :
	m(new VertexArrayPrivate(context))
{
	m->context->m->bindThisContext();

	GLuint id;
	CHECK_GL_ERROR(glGenVertexArrays(1, &id), "Can not create VAO");

	m->id = id;
}



}
