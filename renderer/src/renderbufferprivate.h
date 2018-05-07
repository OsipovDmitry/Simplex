#ifndef RENDERBUFFERPRIVATE_H
#define RENDERBUFFERPRIVATE_H

#include "glutils.h"

namespace renderer {

class Context;
using ContextPtr = std::shared_ptr<Context>;

class RenderbufferPrivate {
public:
	ContextPtr context;
	GLuintPtr id;

	RenderbufferPrivate(ContextPtr context_, GLuintPtr id_) :
		context(context_),
		id(id_)
	{}
};

}

#endif // RENDERBUFFERPRIVATE_H
