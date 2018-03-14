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

}

#endif // BUFFERPRIVATE_H
