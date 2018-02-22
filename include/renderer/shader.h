#ifndef SHADER_H
#define SHADER_H

#include <memory>
#include <string>

#include "renderer_global.h"

namespace renderer {

class Context;
using ContextPtr = std::shared_ptr<Context>;

enum class ShaderType : int32_t {
	Vertex = 0,
	Fragment,
	Count
};

template <typename T> constexpr ShaderType castToShaderType(T value) { return static_cast<ShaderType>(value); }
template <typename T> constexpr T castFromShaderType(ShaderType value) { return static_cast<T>(value); }

class ShaderPrivate;
class RENDERERSHARED_EXPORT Shader {
public:
	~Shader();

	ContextPtr context() const;

	void setSourceCode(const std::string& value);
	std::string sourceCode() const;

	bool compile(std::string *pLog = nullptr);

	ShaderType type() const;

private:
	Shader(ContextPtr context, ShaderType type);

	ShaderPrivate *m;

	friend class Context;
	friend class Program;
};

}

#endif // SHADER_H
