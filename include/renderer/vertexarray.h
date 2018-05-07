#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include <memory>

#include "renderer_global.h"

namespace renderer {

class Context;
using ContextPtr = std::shared_ptr<Context>;

class Buffer;
using BufferPtr = std::shared_ptr<Buffer>;

class VertexArray;
using VertexArrayPtr = std::shared_ptr<VertexArray>;

enum class VertexArrayAttributePointerType : int32_t {
	Type_8ui,
	Type_8i,
	Type_16ui,
	Type_16i,
	Type_32ui,
	Type_32i,
	Type_16f,
	Type_32f,
	Count
};

template <typename T> constexpr VertexArrayAttributePointerType castToVertexArrayAttributePointerType(T value) { return static_cast<VertexArrayAttributePointerType>(value); }
template <typename T> constexpr T castFromVertexArrayAttributePointerType(VertexArrayAttributePointerType value) { return static_cast<T>(value); }

class VertexArrayPrivate;
class RENDERERSHARED_EXPORT VertexArray : public std::enable_shared_from_this<VertexArray> {
public:
	~VertexArray();

	ContextPtr context() const;

	void bindIndexBuffer(BufferPtr buffer);
	void unbindIndexBuffer();

	void bindVertexBuffer(int32_t attribIndex, BufferPtr buffer, int32_t numComponents, VertexArrayAttributePointerType dataType, bool normalize, int32_t dataStride, int32_t dataOffset);
	void bindVertexBufferInteger(int32_t attribIndex, BufferPtr buffer, int32_t numComponents, VertexArrayAttributePointerType dataType, int32_t dataStride, int32_t dataOffset);
	void unbindVertexBuffer(int32_t attribIndex);

//	void bindVertexAttribute(int32_t attribIndex, const glm::vec4& value);
//	void bindVertexAttributeInteger(int32_t attribIndex, const glm::ivec4& value);

	BufferPtr indexBuffer() const;
	BufferPtr vertexBuffer(int32_t attribIndex) const;

private:
	VertexArray(ContextPtr context);
	void initShared(VertexArrayPtr sharedVertexArray);

	VertexArrayPrivate *m;

	friend class Context;
	friend class ContextPrivate;
};

}

#endif // VERTEXARRAY_H
