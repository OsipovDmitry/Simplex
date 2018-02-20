#ifndef SHADER_H
#define SHADER_H

#include <memory>
#include <string>

#include "renderer_global.h"

namespace renderer {

class Context;
using ContextPtr = std::shared_ptr<Context>;

class ShaderPrivate;
class RENDERERSHARED_EXPORT Shader {
public:
	enum class Type {
		Vertex,
		Fragment,
		Count
	};

	~Shader();

	void setSourceCode(const std::string& value);
	std::string sourceCode() const;

	bool compile(std::string *pLog = nullptr);

private:
	Shader(ContextPtr context, Type type);

	ShaderPrivate *m;
};

}

#endif // SHADER_H
