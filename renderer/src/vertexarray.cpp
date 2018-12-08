#include <algorithm>
#include <array>

#include <glm/gtc/type_ptr.hpp>

#include <renderer/context.h>
#include <renderer/buffer.h>
#include <renderer/vertexarray.h>

#include "glutils.h"
#include "context_p.h"
#include "vertexarray_p.h"

namespace renderer {

namespace {

const std::array<GLenum, castFromVertexArrayAttributePointerType(VertexArrayAttributePointerType::Count)> vertexArrayAttributePointerTypeTable{
	GL_UNSIGNED_BYTE, GL_BYTE,
	GL_UNSIGNED_SHORT, GL_SHORT,
	GL_UNSIGNED_INT, GL_INT,
	GL_HALF_FLOAT, GL_FLOAT
};

GLenum toVertexArrayAttributePointerGLType(VertexArrayAttributePointerType val) {
    return vertexArrayAttributePointerTypeTable[castFromVertexArrayAttributePointerType(val)];
}

VertexArrayAttributePointerType fromVertexArrayAttributePointerGLType(GLenum val) {
	return castToVertexArrayAttributePointerType(std::find(vertexArrayAttributePointerTypeTable.cbegin(), vertexArrayAttributePointerTypeTable.cend(), (GLenum)val) - vertexArrayAttributePointerTypeTable.cbegin());
}

}

ContextPtr VertexArray::context() const
{
	return m_->context;
}

void VertexArray::bindIndexBuffer(BufferPtr buffer)
{
//	if (m_->context != buffer->context()) {
//		LOG_CRITICAL("VAO and IBO are in different contexts");
//		return;
//	}

    m_->context->m()->bindThisContext();
    m_->context->m()->bindVertexArray(this);
    m_->context->m()->bindBuffer(buffer.get(), BufferTarget::Element);
    m_->context->m()->bindVertexArray(nullptr);

	m_->indexBuffer = buffer;
}

void VertexArray::unbindIndexBuffer()
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindVertexArray(this);
    m_->context->m()->bindBuffer(nullptr, BufferTarget::Element);
    m_->context->m()->bindVertexArray(nullptr);

	m_->indexBuffer = nullptr;
}

void VertexArray::bindVertexBuffer(int32_t attribIndex, BufferPtr buffer, int32_t numComponents, VertexArrayAttributePointerType dataType, bool normalize, int32_t dataStride, int32_t dataOffset)
{
//	if (m_->context != buffer->context()) {
//		LOG_CRITICAL("VAO and VBO are in different contexts");
//		return;
//	}

    m_->context->m()->bindThisContext();
    m_->context->m()->bindVertexArray(this);
    m_->context->m()->bindBuffer(buffer.get(), BufferTarget::Array);
	CHECK_GL_ERROR(glVertexAttribPointer(attribIndex, numComponents, toVertexArrayAttributePointerGLType(dataType), normalize ? GL_TRUE : GL_FALSE, dataStride, (const void*)dataOffset), "Can not set vertex attribute pointer");
	CHECK_GL_ERROR(glEnableVertexAttribArray(attribIndex), "Can not set vertex attribute pointer");
    m_->context->m()->bindVertexArray(nullptr);

	m_->vertexBuffers[attribIndex] = buffer;
}

void VertexArray::bindVertexBufferInteger(int32_t attribIndex, BufferPtr buffer, int32_t numComponents, VertexArrayAttributePointerType dataType, int32_t dataStride, int32_t dataOffset)
{
//	if (m_->context != buffer->context()) {
//		LOG_CRITICAL("VAO and VBO are in different contexts");
//		return;
//	}

    m_->context->m()->bindThisContext();
    m_->context->m()->bindVertexArray(this);
    m_->context->m()->bindBuffer(buffer.get(), BufferTarget::Array);
	CHECK_GL_ERROR(glVertexAttribIPointer(attribIndex, numComponents, toVertexArrayAttributePointerGLType(dataType), dataStride, (const void*)dataOffset), "Can not set vertex attribute integer pointer");
	CHECK_GL_ERROR(glEnableVertexAttribArray(attribIndex), "Can not set vertex attribute integer pointer");
    m_->context->m()->bindVertexArray(nullptr);

	m_->vertexBuffers[attribIndex] = buffer;
}

void VertexArray::unbindVertexBuffer(int32_t attribIndex)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindVertexArray(this);
    m_->context->m()->bindBuffer(nullptr, BufferTarget::Array);
	CHECK_GL_ERROR(glDisableVertexAttribArray(attribIndex), "Can not unbind vertex attribute pointer");
    m_->context->m()->bindVertexArray(nullptr);

	m_->vertexBuffers.erase(attribIndex);
}

//void VertexArray::bindVertexAttribute(int32_t attribIndex, const glm::vec4& value)
//{
//	m->context->m()->bindThisContext();
//	m->context->m()->bindVertexArray(this);
//	CHECK_GL_ERROR(glVertexAttrib4fv(attribIndex, glm::value_ptr(value)), "Can not set vertex attribute");
//	CHECK_GL_ERROR(glDisableVertexAttribArray(attribIndex), "Can not set vertex attribute");
//	m->context->m()->bindVertexArray(nullptr);
//	m->vertexBuffers[attribIndex] = nullptr;
//}

//void VertexArray::bindVertexAttributeInteger(int32_t attribIndex, const glm::ivec4& value)
//{
//	m->context->m()->bindThisContext();
//	m->context->m()->bindVertexArray(this);
//	CHECK_GL_ERROR(glVertexAttribI4iv(attribIndex, glm::value_ptr(value)), "Can not set vertex attribute integer");
//	CHECK_GL_ERROR(glDisableVertexAttribArray(attribIndex), "Can not set vertex attribute integer");
//	m->context->m()->bindVertexArray(nullptr);
//	m->vertexBuffers[attribIndex] = nullptr;
//}

BufferPtr VertexArray::indexBuffer() const
{
	return m_->indexBuffer;
}

BufferPtr VertexArray::vertexBuffer(int32_t attribIndex) const
{
    return m_->vertexBuffers[attribIndex];
}

VertexArrayPtr VertexArray::create(ContextPtr context)
{
    return VertexArrayPtr(new VertexArray(context), VertexArrayDeleter());
}

VertexArray::VertexArray(ContextPtr context) :
    m_(new VertexArrayPrivate(context))
{
    m_->context->m()->bindThisContext();
    CHECK_GL_ERROR(glGenVertexArrays(1, &m_->id), "Can not create VAO");
}

void VertexArray::initShared(VertexArrayPtr sharedVertexArray)
{
	if (sharedVertexArray->m_->indexBuffer)
        bindIndexBuffer(sharedVertexArray->m_->indexBuffer);

	// TO DO: переписать без смены контекстов в каждой итерации цикла!!!!!!!!!!!!!1
	for (auto iter = sharedVertexArray->m_->vertexBuffers.cbegin(); iter != sharedVertexArray->m_->vertexBuffers.cend(); ++iter) {
		GLint dataNumComponents, dataType, dataNormalized, dataStride, isDataInteger;
		GLvoid *dataPointer;

        sharedVertexArray->m_->context->m()->bindThisContext();
        m_->context->m()->bindVertexArray(sharedVertexArray.get());
		CHECK_GL_ERROR(glGetVertexAttribiv(iter->first, GL_VERTEX_ATTRIB_ARRAY_SIZE, &dataNumComponents), "Can not create VAO");
		CHECK_GL_ERROR(glGetVertexAttribiv(iter->first, GL_VERTEX_ATTRIB_ARRAY_TYPE, &dataType), "Can not create VAO");
		CHECK_GL_ERROR(glGetVertexAttribiv(iter->first, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &dataNormalized), "Can not create VAO");
		CHECK_GL_ERROR(glGetVertexAttribiv(iter->first, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &dataStride), "Can not create VAO");
		CHECK_GL_ERROR(glGetVertexAttribiv(iter->first, GL_VERTEX_ATTRIB_ARRAY_INTEGER, &isDataInteger), "Can not create VAO");
		CHECK_GL_ERROR(glGetVertexAttribPointerv(iter->first, GL_VERTEX_ATTRIB_ARRAY_POINTER, &dataPointer), "Can not create VAO");

		int32_t dataOffset = static_cast<int32_t>(reinterpret_cast<intptr_t>(dataPointer));

		isDataInteger == GL_FALSE ?
                    bindVertexBuffer(iter->first, iter->second, dataNumComponents, fromVertexArrayAttributePointerGLType(dataType), dataNormalized != GL_FALSE, dataStride, dataOffset) :
                    bindVertexBufferInteger(iter->first, iter->second, dataNumComponents, fromVertexArrayAttributePointerGLType(dataType), dataStride, dataOffset);
	}
}

VertexArray::~VertexArray()
{
    m_->context->m()->bindThisContext();
    CHECK_GL_ERROR(glDeleteVertexArrays(1, &m_->id), "Can not delete VAO");
    delete m_;
}

}
