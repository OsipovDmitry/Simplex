#ifndef BUFFER_H
#define BUFFER_H

#include <memory>

#include "../utils/pimpl.h"
#include "../utils/noncopyble.h"
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
class RENDERERSHARED_EXPORT Buffer : public std::enable_shared_from_this<Buffer> {
    PIMPL(Buffer)

public:
	~Buffer();

	ContextPtr context() const;
	BufferUsage usage() const;
	int64_t size() const;

	void *map(BufferAccess access, uint64_t offset = 0, int64_t length = -1);
	bool unmap();

private:
	Buffer(ContextPtr context);
	Buffer(ContextPtr context, BufferPtr sharedBuffer);
	void init(BufferUsage usage, uint64_t size, const void* pData);

    BufferPrivate *m_;

	friend class Context;
	friend class ContextPrivate;
};

}

#endif // BUFFER_H
