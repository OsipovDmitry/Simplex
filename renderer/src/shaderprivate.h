#ifndef SHADERPRIVATE_H
#define SHADERPRIVATE_H

#include "glutils.h"

namespace renderer {

class Context;
using ContextPtr = std::shared_ptr<Context>;

class ShaderPrivate {
public:
	ContextPtr context;
	GLuintPtr id;

	ShaderPrivate(ContextPtr context_, GLuintPtr id_) :
		context(context_),
		id(id_)
	{}
};

}

#endif // SHADERPRIVATE_H
