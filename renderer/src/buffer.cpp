#include <array>
#include <algorithm>

#include <GLES3/gl3.h>

#include <renderer/buffer.h>
#include <renderer/context.h>

#include "contextprivate.h"
#include "bufferprivate.h"
#include "checkglerror.h"

namespace renderer {

namespace {

const std::array<GLenum, castFromBufferUsage<size_t>(BufferUsage::Count)> bufferUsageTable {
	GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY,
	GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, GL_DYNAMIC_COPY,
	GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY
};

GLenum toBufferGLUsage(BufferUsage val) {
	return bufferUsageTable[castFromBufferUsage<size_t>(val)];
}

BufferUsage fromBufferGLUsage(GLenum val) {
	return castToBufferUsage(std::find(bufferUsageTable.cbegin(), bufferUsageTable.cend(), val) - bufferUsageTable.cbegin());
}

}

Buffer::~Buffer()
{
	auto context = m->context;
	auto id = m->id;
	delete m;

	context->m->bindThisContext();
	CHECK_GL_ERROR(glDeleteBuffers(1, &id), "Can not delete buffer");
}

ContextPtr Buffer::context() const
{
	return m->context;
}

BufferUsage Buffer::usage() const
{
	m->context->m->bindThisContext();
	m->context->m->bindBuffer(BufferTarget::Array, shared_from_this());

	BufferUsage usage = BufferUsage::StaticDraw;
	GLint value;
	CHECK_GL_ERROR(glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &value), "Can not get buffer's usage", usage);
	usage = fromBufferGLUsage((GLenum)value);
	return usage;
}

int64_t Buffer::size() const
{
	m->context->m->bindThisContext();
	m->context->m->bindBuffer(BufferTarget::Array, shared_from_this());

	GLint64 value;
	CHECK_GL_ERROR(glGetBufferParameteri64v(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &value), "Can not get buffer's size", 0);
	return value;
}

void *Buffer::map(BufferAccess access, int64_t offset, int64_t length)
{
	if (length < 0)
		length = size() - offset;

	m->context->m->bindThisContext();
	m->context->m->bindBuffer(BufferTarget::Array, shared_from_this());

	GLbitfield GLaccess = GL_MAP_READ_BIT;
	switch (access) {
		case BufferAccess::ReadOnly: { GLaccess = GL_MAP_READ_BIT; break; }
		case BufferAccess::WriteOnly: { GLaccess = GL_MAP_WRITE_BIT; break; }
		case BufferAccess::ReadWrite: { GLaccess = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT; break; }
	}
	CHECK_GL_ERROR(auto result = glMapBufferRange(GL_ARRAY_BUFFER, offset, length, GLaccess), "Can not map buffer", nullptr);
	return result;
}


bool Buffer::unmap()
{
	m->context->m->bindThisContext();
	m->context->m->bindBuffer(BufferTarget::Array, shared_from_this());

	CHECK_GL_ERROR(auto result = glUnmapBuffer(GL_ARRAY_BUFFER), "Can not unmap buffer", false);
	return result != GL_FALSE;
}

Buffer::Buffer(ContextPtr context) :
	m(new BufferPrivate(context))
{
	m->context->m->bindThisContext();

	GLuint id;
	CHECK_GL_ERROR(glGenBuffers(1, &id), "Can not create buffer");

	m->id = id;
}

void Buffer::init(BufferUsage usage, uint64_t size, const void* pData)
{
	m->context->m->bindBuffer(BufferTarget::Array, shared_from_this());
	CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, size, pData, toBufferGLUsage(usage)), "Can not init buffer");
}

}
