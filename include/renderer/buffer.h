#ifndef BUFFER_H
#define BUFFER_H

#include <memory>

#include "../utils/pimpl.h"
#include "../utils/noncopyble.h"
#include "../utils/customdeleter.h"
#include "../utils/enumclass.h"
#include "renderer_global.h"
#include "forwarddecl.h"

namespace renderer {

ENUMCLASS(BufferUsage, int32_t,
          StaticDraw, StaticRead, StaticCopy,
          DynamicDraw, DynamicRead, DynamicCopy,
          StreamDraw, StreamRead, StreamCopy)

ENUMCLASS(BufferAccess, int32_t,
          ReadOnly, WriteOnly, ReadWrite)

class BufferPrivate;
class RENDERERSHARED_EXPORT Buffer {
    PIMPL(Buffer)
    NONCOPYBLE(Buffer)
    CUSTOMDELETER(Buffer)

public:
	ContextPtr context() const;
	BufferUsage usage() const;
	int64_t size() const;

	void *map(BufferAccess access, uint64_t offset = 0, int64_t length = -1);
	bool unmap();

    static BufferPtr create(ContextPtr context, BufferUsage usage, uint64_t size = 0, const void *pData = nullptr);

private:
    Buffer(ContextPtr context, BufferUsage usage, uint64_t size, const void* pData);
    ~Buffer();

    BufferPrivate *m_;
};

}

#endif // BUFFER_H
