#ifndef SHADERPRIVATE_H
#define SHADERPRIVATE_H

#include <GLES3/gl3.h>

#include <renderer/forwarddecl.h>

namespace renderer {

class ShaderPrivate {
public:
	ContextPtr context;
    GLuint id;

    ShaderPrivate(ContextPtr context_) :
		context(context_),
        id(0)
	{}
};

}

#endif // SHADERPRIVATE_H
