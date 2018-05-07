#ifndef BUFFERPRIVATE_H
#define BUFFERPRIVATE_H

#include "glutils.h"

namespace renderer {

class Context;
using ContextPtr = std::shared_ptr<Context>;

class BufferPrivate {
public:
	ContextPtr context;
	GLuintPtr id;

	BufferPrivate(ContextPtr context_, GLuintPtr id_) :
		context(context_),
		id(id_)
	{}

};

enum class BufferTarget : int32_t {
	Array = 0,
	Element,
	CopyRead,
	CopyWrite,
	PixelPack,
	PixelUnpack,
	TransformFeedback,
	Uniform, // 32 slots for binding points
	Count = Uniform+32
};
template <typename T> constexpr BufferTarget castToBufferTarget(T value) { return static_cast<BufferTarget>(value); }
template <typename T> constexpr T castFromBufferTarget(BufferTarget value) { return static_cast<T>(value); }


GLenum toBufferGLTarget(BufferTarget val);
BufferTarget fromBufferGLTarget(GLenum val);

}

#endif // BUFFERPRIVATE_H
