#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include <memory>

#include "../utils/pimpl.h"
#include "../utils/noncopyble.h"
#include "../utils/customdeleter.h"
#include "../utils/enumclass.h"
#include "renderer_global.h"
#include "forwarddecl.h"

namespace renderer {

ENUMCLASS(VertexArrayAttributePointerType, int32_t,
	Type_8ui,
	Type_8i,
	Type_16ui,
	Type_16i,
	Type_32ui,
	Type_32i,
	Type_16f,
    Type_32f
)

class VertexArrayPrivate;
class RENDERERSHARED_EXPORT VertexArray {
    PIMPL(VertexArray)
    NONCOPYBLE(VertexArray)
    CUSTOMDELETER(VertexArray)

public:
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


    static VertexArrayPtr create(ContextPtr context);
private:
	VertexArray(ContextPtr context);
	void initShared(VertexArrayPtr sharedVertexArray);
    ~VertexArray();

    VertexArrayPrivate *m_;
};

}

#endif // VERTEXARRAY_H
