#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include <memory>

#include "renderer_global.h"

namespace renderer {

class Context;
using ContextPtr = std::shared_ptr<Context>;

class Buffer;
using BufferPtr = std::shared_ptr<Buffer>;

enum class VertexArrayAttributePointerType : int32_t {
	Int8,
	Uint8,
	Int16,
	Uint16,
	Int32,
	Uint32,
	Int_2_10_10_10_Rev,
	Uint_2_10_10_10_Rev,
	HalfFloat,
	Float,
	Fixed,
};

class VertexArrayPrivate;
class RENDERERSHARED_EXPORT VertexArray : public std::enable_shared_from_this<VertexArray> {
public:
	~VertexArray();

	void bindIndexBuffer(BufferPtr buffer);
	void bindVertexBuffer(int32_t index, int32_t numComponents, BufferPtr buffer);

private:
	VertexArray(ContextPtr context);

	VertexArrayPrivate *m;

	friend class Context;
	friend class ContextPrivate;
};

}

#endif // VERTEXARRAY_H
