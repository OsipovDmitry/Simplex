#include <algorithm>

#include <renderer/shader.h>
#include <renderer/context.h>

#include "glutils.h"
#include "context_p.h"
#include "shader_p.h"

namespace renderer {

namespace {

const std::array<GLenum, castFromShaderType(ShaderType::Count)> shaderTypeTable{
    GL_VERTEX_SHADER,
    GL_FRAGMENT_SHADER
};

GLenum toShaderGLType(ShaderType val) {
    return shaderTypeTable[castFromShaderType(val)];
}

ShaderType fromShaderGLType(GLenum val) {
	return castToShaderType(std::find(shaderTypeTable.cbegin(), shaderTypeTable.cend(), (GLenum)val) - shaderTypeTable.cbegin());
}

}

Shader::~Shader()
{
    m_->context->m()->bindThisContext();
    CHECK_GL_ERROR(glDeleteShader(m_->id), "Can not delete shader");
    delete m_;
}

ContextPtr Shader::context() const
{
    return m_->context;
}

void Shader::setSourceCode(const std::string& value)
{
    m_->context->m()->bindThisContext();
	auto data = value.c_str();
    CHECK_GL_ERROR(glShaderSource(m_->id, 1, &data, nullptr), "Can not set shader's source code");
}

std::string Shader::sourceCode() const
{
    m_->context->m()->bindThisContext();
	GLint len;
    CHECK_GL_ERROR(glGetShaderiv(m_->id, GL_SHADER_SOURCE_LENGTH, &len), "Can not get shader's source code", std::string());
	std::string ret(len, 0);
    CHECK_GL_ERROR(glGetShaderSource(m_->id, len, /*&len*/nullptr, &(ret[0])), "Can not get shader's source code", std::string());
	return ret;
}

bool Shader::compile(std::string* pLog)
{
    m_->context->m()->bindThisContext();
    CHECK_GL_ERROR(glCompileShader(m_->id), "Can not compile shader", false);

	GLint res;
    CHECK_GL_ERROR(glGetShaderiv(m_->id, GL_COMPILE_STATUS, &res), "Can not compile shader", false);

	if (pLog && (res == GL_FALSE)) {
		GLint len;
        CHECK_GL_ERROR(glGetShaderiv(m_->id, GL_INFO_LOG_LENGTH, &len), "Can not compile shader", false);
		pLog->resize(len);
        CHECK_GL_ERROR(glGetShaderInfoLog(m_->id, len, nullptr, &((*pLog)[0])), "Can not compile shader", false);
	}

    return res != GL_FALSE;
}

ShaderPtr Shader::create(ContextPtr context, ShaderType type)
{
    return ShaderPtr(new Shader(context, type), ShaderDeleter());
}

ShaderType Shader::type() const
{
    m_->context->m()->bindThisContext();
	GLint res;
    CHECK_GL_ERROR(glGetShaderiv(m_->id, GL_SHADER_TYPE, &res), "Can not get shader type", ShaderType::Count);

	return fromShaderGLType(res);
}

Shader::Shader(ContextPtr context, ShaderType type) :
    m_(new ShaderPrivate(context))
{
    m_->context->m()->bindThisContext();
    CHECK_GL_ERROR(m_->id = glCreateShader(toShaderGLType(type)), "Can not create shader");
}

}
