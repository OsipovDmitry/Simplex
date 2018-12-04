#ifndef PROGRAMPRIVATE_H
#define PROGRAMPRIVATE_H

#include <GLES3/gl3.h>

#include <renderer/forwarddecl.h>
#include <renderer/shader.h>

namespace renderer {

class ProgramPrivate {
public:
	ContextPtr context;
    std::array<ShaderPtr, castFromShaderType(ShaderType::Count)> shaders;
    GLuint id;

    ProgramPrivate(ContextPtr context_) :
		context(context_),
		shaders(),
        id(0)
	{}
};

}

#endif // PROGRAMPRIVATE_H
