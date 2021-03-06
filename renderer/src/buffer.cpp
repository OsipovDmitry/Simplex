#include <array>
#include <algorithm>

#include <renderer/buffer.h>
#include <renderer/context.h>

#include "glutils.h"
#include "context_p.h"
#include "buffer_p.h"

namespace renderer {

namespace {

const std::array<GLenum, castFromBufferUsage(BufferUsage::Count)> bufferUsageTable {
	GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY,
    GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, GL_DYNAMIC_COPY,
	GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY
};

GLenum toBufferGLUsage(BufferUsage val) {
    return bufferUsageTable[castFromBufferUsage(val)];
}

BufferUsage fromBufferGLUsage(GLenum val) {
	return castToBufferUsage(std::find(bufferUsageTable.cbegin(), bufferUsageTable.cend(), val) - bufferUsageTable.cbegin());
}

}

ContextPtr Buffer::context() const
{
    return m_->context;
}

BufferUsage Buffer::usage() const
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindBuffer(this, BufferTarget::Array);

	BufferUsage usage = BufferUsage::StaticDraw;
	GLint value;
	CHECK_GL_ERROR(glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &value), "Can not get buffer's usage", usage);
	usage = fromBufferGLUsage((GLenum)value);
	return usage;
}

int64_t Buffer::size() const
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindBuffer(this, BufferTarget::Array);

	GLint64 value;
	CHECK_GL_ERROR(glGetBufferParameteri64v(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &value), "Can not get buffer's size", 0);
	return value;
}

void *Buffer::map(BufferAccess access, uint64_t offset, int64_t length)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindBuffer(this, BufferTarget::Array);

	if (length < 0)
		length = size() - offset;

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
    m_->context->m()->bindThisContext();
    m_->context->m()->bindBuffer(this, BufferTarget::Array);

	CHECK_GL_ERROR(auto result = glUnmapBuffer(GL_ARRAY_BUFFER), "Can not unmap buffer", false);
    return result != GL_FALSE;
}

BufferPtr Buffer::create(ContextPtr context, BufferUsage usage, uint64_t size, const void *pData)
{
    return BufferPtr(new Buffer(context, usage, size, pData), BufferDeleter());
}

Buffer::Buffer(ContextPtr context, BufferUsage usage, uint64_t size, const void* pData) :
    m_(new BufferPrivate(context))
{
    m_->context->m()->bindThisContext();

    CHECK_GL_ERROR(glGenBuffers(1, &m_->id), "Can not create buffer");
    m_->context->m()->bindBuffer(this, BufferTarget::Array);
    CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, size, pData, toBufferGLUsage(usage)), "Can not init buffer");
}

Buffer::~Buffer()
{
    m_->context->m()->bindThisContext();
    CHECK_GL_ERROR(glDeleteBuffers(1, &m_->id), "Can not delete buffer");
    delete m_;
}

}
