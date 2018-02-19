#ifndef CONTEXT_H
#define CONTEXT_H

#include "renderer_global.h"

namespace renderer {

class ContextPrivate;
class RENDERERSHARED_EXPORT Context {
public:
private:
	Context();
	~Context();

	ContextPrivate *m;
};

}

#endif
