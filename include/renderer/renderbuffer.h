#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include <memory>
#include <string>

#include "renderer_global.h"
#include "forwarddecl.h"
#include "../utils/pimpl.h"
#include "../utils/noncopyble.h"
#include "../utils/customdeleter.h"

namespace renderer {

class RenderbufferPrivate;
class RENDERERSHARED_EXPORT Renderbuffer {
    PIMPL(Renderbuffer)
    NONCOPYBLE(Renderbuffer)
    CUSTOMDELETER(Renderbuffer)

public:
	ContextPtr context() const;
	TextureInternalFormat internalFormat() const;
	uint32_t width();
	uint32_t height();

    static RenderbufferPtr create(ContextPtr context, TextureInternalFormat internalFormat, uint32_t width, uint32_t height);

private:
    Renderbuffer(ContextPtr context, TextureInternalFormat internalFormat, uint32_t width, uint32_t height);
    ~Renderbuffer();

    RenderbufferPrivate *m_;
};

}

#endif // RENDERBUFFER_H
