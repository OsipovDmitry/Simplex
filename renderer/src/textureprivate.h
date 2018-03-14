#ifndef TEXTUREPRIVATE_H
#define TEXTUREPRIVATE_H

#include <renderer/texture.h>
#include "glutils.h"

namespace renderer {

class TexturePrivate {
public:
	ContextPtr context;
	GLuintPtr id;
	TextureType type;

	TexturePrivate(ContextPtr context_, GLuintPtr id_, TextureType type_) :
		context(context_),
		id(id_),
		type(type_)
	{}
};

}

#endif // TEXTUREPRIVATE_H
