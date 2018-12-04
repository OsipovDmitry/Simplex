#ifndef RENDERBUFFERPRIVATE_H
#define RENDERBUFFERPRIVATE_H

#include <GLES3/gl3.h>

#include <renderer/forwarddecl.h>

namespace renderer {

class RenderbufferPrivate {
public:
	ContextPtr context;
    GLuint id;

    RenderbufferPrivate(ContextPtr context_) :
		context(context_),
        id(0)
	{}
};

}

#endif // RENDERBUFFERPRIVATE_H
