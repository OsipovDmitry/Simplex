#ifndef SHADERPRIVATE_H
#define SHADERPRIVATE_H

#include <memory>
#include <GLES3/gl3.h>

namespace renderer {

class Context;
using ContextPtr = std::shared_ptr<Context>;

class ShaderPrivate {
public:
	ContextPtr context;
	GLuint id;

	ShaderPrivate(ContextPtr c) :
		context(c),
		id(0)
	{}
};

}

#endif // SHADERPRIVATE_H
