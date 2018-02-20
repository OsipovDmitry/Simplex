#include <array>

#include <GLES3/gl3.h>

#include <renderer/shader.h>
#include <renderer/context.h>

#include "checkglerror.h"

namespace renderer {

class ShaderPrivate {
public:
	ContextPtr context;
	GLuint id;

	ShaderPrivate(ContextPtr c) :
		context(c),
		id(0)
	{}

	static GLenum glType(Shader::Type val) {
		static const std::array<GLenum, static_cast<size_t>(Shader::Type::Count)> table{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
		return table[static_cast<size_t>(val)];
	}
};

Shader::~Shader()
{
	auto id = m->id;
	delete m;

	Context::makeContextCurrent(m->context);
	CHECK_GL_ERROR(glDeleteShader(id), "Can not delete shader");
}

void Shader::setSourceCode(const std::string& value)
{
	Context::makeContextCurrent(m->context);
	auto data = value.c_str();
	CHECK_GL_ERROR(glShaderSource(m->id, 1, &data, nullptr), "Can not set shader's source code");
}

std::string Shader::sourceCode() const
{
	Context::makeContextCurrent(m->context);
	GLint len;
	CHECK_GL_ERROR(glGetShaderiv(m->id, GL_SHADER_SOURCE_LENGTH, &len), "Can not get shader's source code", std::string());
	std::string ret(len, 0);
	CHECK_GL_ERROR(glGetShaderSource(m->id, len, /*&len*/nullptr, &(ret[0])), "Can not get shader's source code", std::string());
	return ret;
}

bool Shader::compile(std::string* pLog)
{
	Context::makeContextCurrent(m->context);
	CHECK_GL_ERROR(glCompileShader(m->id), "Can not compile shader", false);

	GLint res;
	CHECK_GL_ERROR(glGetShaderiv(m->id, GL_COMPILE_STATUS, &res), "Can not compile shader", false);

	if (pLog && (res == GL_FALSE)) {
		GLint len;
		CHECK_GL_ERROR(glGetShaderiv(m->id, GL_INFO_LOG_LENGTH, &len), "Can not compile shader", false);
		pLog->resize(len);
		CHECK_GL_ERROR(glGetShaderInfoLog(m->id, len, nullptr, &((*pLog)[0])), "Can not compile shader", false);
	}

	return res != GL_FALSE;
}

Shader::Shader(ContextPtr context, Shader::Type type) :
	m(new ShaderPrivate(context))
{
	Context::makeContextCurrent(m->context);
	CHECK_GL_ERROR(auto id = glCreateShader(ShaderPrivate::glType(type)), "Can not create shader");

	m->id = id;
}

}
