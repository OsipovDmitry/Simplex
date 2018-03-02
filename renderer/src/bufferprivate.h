#ifndef BUFFERPRIVATE_H
#define BUFFERPRIVATE_H

#include <memory>

#include <GLES3/gl3.h>

namespace renderer {

class Context;
using ContextPtr = std::shared_ptr<Context>;

class BufferPrivate {
public:
	ContextPtr context;
	GLuint id;

	BufferPrivate(ContextPtr c) :
		context(c),
		id(0)
	{}
};

}

#endif // BUFFERPRIVATE_H
