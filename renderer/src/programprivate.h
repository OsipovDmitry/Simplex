#ifndef PROGRAMPRIVATE_H
#define PROGRAMPRIVATE_H

#include <renderer/shader.h>

#include "glutils.h"

namespace renderer {

class Context;
using ContextPtr = std::shared_ptr<Context>;

class ProgramPrivate {
public:
	ContextPtr context;
	std::array<ShaderPtr, castFromShaderType<size_t>(ShaderType::Count)> shaders;
	GLuintPtr id;

	ProgramPrivate(ContextPtr context_, GLuintPtr id_) :
		context(context_),
		shaders(),
		id(id_)
	{}
};

}

#endif // PROGRAMPRIVATE_H
