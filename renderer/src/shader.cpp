#include <algorithm>

#include <GLES3/gl3.h>

#include <renderer/shader.h>
#include <renderer/context.h>

#include "shaderprivate.h"
#include "checkglerror.h"

namespace renderer {

namespace {

static GLenum shaderGLType(ShaderType val) {
	static const std::array<GLenum, castFromShaderType<size_t>(ShaderType::Count)> table{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
	return table[castFromShaderType<size_t>(val)];
}

}

Shader::~Shader()
{
	auto id = m->id;
	auto context = m->context;
	delete m;

	Context::makeContextCurrent(context);
	CHECK_GL_ERROR(glDeleteShader(id), "Can not delete shader");
}

ContextPtr Shader::context() const
{
	return m->context;
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

ShaderType Shader::type() const
{
	static const std::array<GLenum, castFromShaderType<size_t>(ShaderType::Count)> table{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

	Context::makeContextCurrent(m->context);
	GLint res;
	CHECK_GL_ERROR(glGetShaderiv(m->id, GL_SHADER_TYPE, &res), "Can not get shader type", ShaderType::Count);

	return castToShaderType(std::find(table.cbegin(), table.cend(), (GLenum)res) - table.cbegin());
}

Shader::Shader(ContextPtr context, ShaderType type) :
	m(new ShaderPrivate(context))
{
	Context::makeContextCurrent(m->context);
	CHECK_GL_ERROR(auto id = glCreateShader(shaderGLType(type)), "Can not create shader");

	m->id = id;
}

}
