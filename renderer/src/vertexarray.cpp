#include <algorithm>
#include <array>

#include <glm/gtc/type_ptr.hpp>

#include <renderer/context.h>
#include <renderer/buffer.h>
#include <renderer/vertexarray.h>

#include "contextprivate.h"
#include "vertexarrayprivate.h"

namespace renderer {

namespace {

const std::array<GLenum, castFromVertexArrayAttributePointerType<size_t>(VertexArrayAttributePointerType::Count)> vertexArrayAttributePointerTypeTable{
	GL_BYTE, GL_UNSIGNED_BYTE,
	GL_SHORT, GL_UNSIGNED_SHORT,
	GL_INT, GL_UNSIGNED_INT,
	GL_INT_2_10_10_10_REV, GL_UNSIGNED_INT_2_10_10_10_REV,
	GL_HALF_FLOAT,
	GL_FLOAT,
	GL_FIXED
};

GLenum toVertexArrayAttributePointerGLType(VertexArrayAttributePointerType val) {
	return vertexArrayAttributePointerTypeTable[castFromVertexArrayAttributePointerType<size_t>(val)];
}

VertexArrayAttributePointerType fromVertexArrayAttributePointerGLType(GLenum val) {
	return castToVertexArrayAttributePointerType(std::find(vertexArrayAttributePointerTypeTable.cbegin(), vertexArrayAttributePointerTypeTable.cend(), (GLenum)val) - vertexArrayAttributePointerTypeTable.cbegin());
}

}

VertexArray::~VertexArray()
{
	m->context->m->bindThisContext();
	delete m;
}

void VertexArray::bindIndexBuffer(BufferPtr buffer)
{
	if (m->context != buffer->context()) {
		LOG_CRITICAL("VAO and IBO are in different contexts");
		return;
	}

	m->context->m->bindThisContext();
	m->context->m->bindVertexArray(shared_from_this());
	m->context->m->bindBuffer(buffer, BufferTarget::Element);
	m->context->m->bindVertexArray(nullptr);

	m->indexBuffer = buffer;
}

void VertexArray::unbindIndexBuffer()
{
	m->context->m->bindThisContext();
	m->context->m->bindVertexArray(shared_from_this());
	m->context->m->bindBuffer(nullptr, BufferTarget::Element);
	m->context->m->bindVertexArray(nullptr);

	m->indexBuffer = nullptr;
}

void VertexArray::bindVertexBuffer(int32_t attribIndex, BufferPtr buffer, int32_t numComponents, VertexArrayAttributePointerType dataType, bool normalize, int32_t dataStride, int32_t dataOffset)
{
	if (m->context != buffer->context()) {
		LOG_CRITICAL("VAO and VBO are in different contexts");
		return;
	}

	m->context->m->bindThisContext();
	m->context->m->bindVertexArray(shared_from_this());
	m->context->m->bindBuffer(buffer, BufferTarget::Array);
	CHECK_GL_ERROR(glVertexAttribPointer(attribIndex, numComponents, toVertexArrayAttributePointerGLType(dataType), normalize ? GL_TRUE : GL_FALSE, dataStride, (const void*)dataOffset), "Can not set vertex attribute pointer");
	CHECK_GL_ERROR(glEnableVertexAttribArray(attribIndex), "Can not set vertex attribute pointer");
	m->context->m->bindVertexArray(nullptr);

	m->vertexBuffers[attribIndex] = buffer;
}

void VertexArray::bindVertexBufferInteger(int32_t attribIndex, BufferPtr buffer, int32_t numComponents, VertexArrayAttributePointerType dataType, int32_t dataStride, int32_t dataOffset)
{
	if (m->context != buffer->context()) {
		LOG_CRITICAL("VAO and VBO are in different contexts");
		return;
	}

	m->context->m->bindThisContext();
	m->context->m->bindVertexArray(shared_from_this());
	m->context->m->bindBuffer(buffer, BufferTarget::Array);
	CHECK_GL_ERROR(glVertexAttribIPointer(attribIndex, numComponents, toVertexArrayAttributePointerGLType(dataType), dataStride, (const void*)dataOffset), "Can not set vertex attribute integer pointer");
	CHECK_GL_ERROR(glEnableVertexAttribArray(attribIndex), "Can not set vertex attribute integer pointer");
	m->context->m->bindVertexArray(nullptr);

	m->vertexBuffers[attribIndex] = buffer;
}

void VertexArray::unbindVertexBuffer(int32_t attribIndex)
{
	m->context->m->bindThisContext();
	m->context->m->bindVertexArray(shared_from_this());
	m->context->m->bindBuffer(nullptr, BufferTarget::Array);
	CHECK_GL_ERROR(glDisableVertexAttribArray(attribIndex), "Can not unbind vertex attribute pointer");
	m->context->m->bindVertexArray(nullptr);

	m->vertexBuffers.erase(attribIndex);
}

//void VertexArray::bindVertexAttribute(int32_t attribIndex, const glm::vec4& value)
//{
//	m->context->m->bindThisContext();
//	m->context->m->bindVertexArray(shared_from_this());
//	CHECK_GL_ERROR(glVertexAttrib4fv(attribIndex, glm::value_ptr(value)), "Can not set vertex attribute");
//	CHECK_GL_ERROR(glDisableVertexAttribArray(attribIndex), "Can not set vertex attribute");
//	m->context->m->bindVertexArray(nullptr);
//	m->vertexBuffers[attribIndex] = nullptr;
//}

//void VertexArray::bindVertexAttributeInteger(int32_t attribIndex, const glm::ivec4& value)
//{
//	m->context->m->bindThisContext();
//	m->context->m->bindVertexArray(shared_from_this());
//	CHECK_GL_ERROR(glVertexAttribI4iv(attribIndex, glm::value_ptr(value)), "Can not set vertex attribute integer");
//	CHECK_GL_ERROR(glDisableVertexAttribArray(attribIndex), "Can not set vertex attribute integer");
//	m->context->m->bindVertexArray(nullptr);
//	m->vertexBuffers[attribIndex] = nullptr;
//}

BufferPtr VertexArray::indexBuffer() const
{
	return m->indexBuffer;
}

BufferPtr VertexArray::vertexBuffer(int32_t attribIndex) const
{
	return m->vertexBuffers[attribIndex];
}

VertexArray::VertexArray(ContextPtr context) :
	m(new VertexArrayPrivate(context, nullptr))
{
	m->context->m->bindThisContext();

	auto id = new GLuint(0);
	CHECK_GL_ERROR(glGenVertexArrays(1, id), "Can not create VAO");

	m->id = GLuintPtr(id, [](GLuint *p) {
		// context bind in destructor of class
		CHECK_GL_ERROR(glDeleteVertexArrays(1, p), "Can not delete VAO");
		delete p;
	});
}

void VertexArray::initShared(VertexArrayPtr sharedVertexArray)
{
	if (sharedVertexArray->m->indexBuffer)
		bindIndexBuffer(m->context->createSharedBuffer(sharedVertexArray->m->indexBuffer));

	for (auto iter = sharedVertexArray->m->vertexBuffers.cbegin(); iter != sharedVertexArray->m->vertexBuffers.cend(); ++iter) {
		GLint dataNumComponents, dataType, dataNormalized, dataStride, isDataInteger;
		GLvoid *dataPointer;

		sharedVertexArray->m->context->m->bindThisContext();
		m->context->m->bindVertexArray(sharedVertexArray);
		CHECK_GL_ERROR(glGetVertexAttribiv(iter->first, GL_VERTEX_ATTRIB_ARRAY_SIZE, &dataNumComponents), "Can not create VAO");
		CHECK_GL_ERROR(glGetVertexAttribiv(iter->first, GL_VERTEX_ATTRIB_ARRAY_TYPE, &dataType), "Can not create VAO");
		CHECK_GL_ERROR(glGetVertexAttribiv(iter->first, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &dataNormalized), "Can not create VAO");
		CHECK_GL_ERROR(glGetVertexAttribiv(iter->first, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &dataStride), "Can not create VAO");
		CHECK_GL_ERROR(glGetVertexAttribiv(iter->first, GL_VERTEX_ATTRIB_ARRAY_INTEGER, &isDataInteger), "Can not create VAO");
		CHECK_GL_ERROR(glGetVertexAttribPointerv(iter->first, GL_VERTEX_ATTRIB_ARRAY_POINTER, &dataPointer), "Can not create VAO");

		int32_t dataOffset = static_cast<int32_t>(reinterpret_cast<intptr_t>(dataPointer));

		isDataInteger == GL_FALSE ?
					bindVertexBuffer(iter->first, m->context->createSharedBuffer(iter->second), dataNumComponents, fromVertexArrayAttributePointerGLType(dataType), dataNormalized != GL_FALSE, dataStride, dataOffset) :
					bindVertexBufferInteger(iter->first, m->context->createSharedBuffer(iter->second), dataNumComponents, fromVertexArrayAttributePointerGLType(dataType), dataStride, dataOffset);
	}
}



}
