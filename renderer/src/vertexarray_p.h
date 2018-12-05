#ifndef VERTEXARRAYPRIVATE_H
#define VERTEXARRAYPRIVATE_H

#include <map>

#include <GLES3/gl3.h>

#include <renderer/forwarddecl.h>

namespace renderer {

class VertexArrayPrivate {
public:
	ContextPtr context;
	BufferPtr indexBuffer;
	std::map<int32_t, BufferPtr> vertexBuffers;
    GLuint id;

    VertexArrayPrivate(ContextPtr context_) :
		context(context_),
		indexBuffer(),
		vertexBuffers(),
        id(0)
	{}
};

}

#endif // VERTEXARRAYPRIVATE_H
