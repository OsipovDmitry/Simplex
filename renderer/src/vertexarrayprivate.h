#ifndef VERTEXARRAYPRIVATE_H
#define VERTEXARRAYPRIVATE_H

#include <memory>

#include <GLES3/gl3.h>

namespace renderer {

class Context;
using ContextPtr = std::shared_ptr<Context>;

class VertexArrayPrivate {
public:
	ContextPtr context;
	GLuint id;

	VertexArrayPrivate(ContextPtr c) :
		context(c),
		id(0)
	{}
};

}

#endif // VERTEXARRAYPRIVATE_H
