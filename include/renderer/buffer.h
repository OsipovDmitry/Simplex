#ifndef BUFFER_H
#define BUFFER_H

#include <memory>

#include "renderer_global.h"

namespace renderer {

class Context;
using ContextPtr = std::shared_ptr<Context>;

enum class BufferUsage : int32_t {
	StaticDraw = 0, StaticRead, StaticCopy,
	DynamicDraw, DynamicRead, DynamicCopy,
	StreamDraw, StreamRead, StreamCopy,
	Count
};
template <typename T> constexpr BufferUsage castToBufferUsage(T value) { return static_cast<BufferUsage>(value); }
template <typename T> constexpr T castFromBufferUsage(BufferUsage value) { return static_cast<T>(value); }

enum class BufferAccess : int32_t {
	ReadOnly = 0, WriteOnly, ReadWrite,
	Count
};
template <typename T> constexpr BufferAccess castToBufferAccess(T value) { return static_cast<BufferAccess>(value); }
template <typename T> constexpr T castFromBufferAccess(BufferAccess value) { return static_cast<T>(value); }

class BufferPrivate;
class RENDERERSHARED_EXPORT Buffer : public std::enable_shared_from_this<Buffer> {
public:
	~Buffer();

	ContextPtr context() const;
	BufferUsage usage() const;
	int64_t size() const;

	void *map(BufferAccess access, int64_t offset = 0, int64_t length = -1);
	bool unmap();

private:
	Buffer(ContextPtr context);
	void init(BufferUsage usage, uint64_t size, const void* pData);

	BufferPrivate *m;

	friend class Context;
	friend class ContextPrivate;
};

}

#endif // BUFFER_H
