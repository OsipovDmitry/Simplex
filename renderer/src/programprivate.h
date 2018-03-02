#ifndef PROGRAMPRIVATE_H
#define PROGRAMPRIVATE_H

#include <renderer/shader.h>

#include <memory>
#include <GLES3/gl3.h>

namespace renderer {

class Context;
using ContextPtr = std::shared_ptr<Context>;

class Program;
using ProgramPtr = std::shared_ptr<Program>;

class ProgramPrivate {
public:
	ContextPtr context;
	std::array<ShaderPtr, castFromShaderType<size_t>(ShaderType::Count)> shaders;
	static std::weak_ptr<Program> currentProgram;
	GLuint id;

	ProgramPrivate(ContextPtr c) :
		context(c),
		shaders(),
		id(0)
	{}
};

}

#endif // PROGRAMPRIVATE_H
