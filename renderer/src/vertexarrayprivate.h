#ifndef VERTEXARRAYPRIVATE_H
#define VERTEXARRAYPRIVATE_H

#include <map>

#include "glutils.h"

namespace renderer {

class Context;
using ContextPtr = std::shared_ptr<Context>;

class Buffer;
using BufferPtr = std::shared_ptr<Buffer>;

class VertexArrayPrivate {
public:
	ContextPtr context;
	BufferPtr indexBuffer;
	std::map<int32_t, BufferPtr> vertexBuffers;
	GLuintPtr id;

	VertexArrayPrivate(ContextPtr context_, GLuintPtr id_) :
		context(context_),
		indexBuffer(),
		vertexBuffers(),
		id(id_)
	{}
};

}

#endif // VERTEXARRAYPRIVATE_H
