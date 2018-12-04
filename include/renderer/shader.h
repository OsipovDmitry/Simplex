#ifndef SHADER_H
#define SHADER_H

#include <memory>
#include <string>

#include "../utils/enumclass.h"
#include "../utils/pimpl.h"
#include "../utils/noncopyble.h"
#include "../utils/customdeleter.h"

#include "renderer_global.h"
#include "forwarddecl.h"

namespace renderer {

ENUMCLASS(ShaderType, int32_t, Vertex, Fragment)

class ShaderPrivate;
class RENDERERSHARED_EXPORT Shader {
    PIMPL(Shader)
    NONCOPYBLE(Shader)
    CUSTOMDELETER(Shader)

public:
	ContextPtr context() const;
    ShaderType type() const;

	void setSourceCode(const std::string& value);
	std::string sourceCode() const;

	bool compile(std::string *pLog = nullptr);

    static ShaderPtr create(ContextPtr context, ShaderType type);

private:
    Shader(ContextPtr context, ShaderType type);
    ~Shader();

    ShaderPrivate *m_;
};

}

#endif // SHADER_H
