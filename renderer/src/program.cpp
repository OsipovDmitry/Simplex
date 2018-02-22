#include <array>

#include <glm/gtc/type_ptr.hpp>
#include <GLES3/gl3.h>

#include <renderer/program.h>
#include <renderer/shader.h>
#include <renderer/context.h>

#include "shaderprivate.h"
#include "checkglerror.h"

namespace renderer {

class ProgramPrivate {
public:
	ContextPtr context;
	std::array<ShaderPtr, castFromShaderType<size_t>(ShaderType::Count)> shaders;
	static std::weak_ptr<Program> currentProgram;
	GLuint id;

	ProgramPrivate(ContextPtr c) :
		context(c),
		id(0),
		shaders()
	{}
};
std::weak_ptr<Program> ProgramPrivate::currentProgram;

Program::~Program()
{
	auto id = m->id;
	auto context = m->context;
	delete m;

	Context::makeContextCurrent(context);
	CHECK_GL_ERROR(glDeleteProgram(id), "Can not delete shader");
}

int Program::id() const
{
	return m->id;
}

void Program::detachShader(ShaderType type)
{
	Context::makeContextCurrent(m->context);
	auto& p = m->shaders[castFromShaderType<size_t>(type)];
	if (p) {
		CHECK_GL_ERROR(glDetachShader(m->id, p->m->id), "Can not detach shader");
		p.reset();
	}
}

void Program::attachShader(ShaderPtr shader)
{
	if (m->context != shader->context()) {
		LOG_CRITICAL("Program and shader are in different contexts");
		return;
	}

	Context::makeContextCurrent(m->context);
	auto type = shader->type();
	detachShader(type);
	CHECK_GL_ERROR(glAttachShader(m->id, shader->m->id), "Can not attach shader");
	m->shaders[castFromShaderType<size_t>(type)] = shader;
}

bool Program::link(std::string* pLog)
{
	Context::makeContextCurrent(m->context);
	CHECK_GL_ERROR(glLinkProgram(m->id), "Can not link program", false);

	GLint res;
	CHECK_GL_ERROR(glGetProgramiv(m->id, GL_LINK_STATUS, &res), "Can not link program", false);

	if (pLog && (res == GL_FALSE)) {
		GLint len;
		CHECK_GL_ERROR(glGetProgramiv(m->id, GL_INFO_LOG_LENGTH, &len), "Can not link program", false);
		pLog->resize(len);
		CHECK_GL_ERROR(glGetProgramInfoLog(m->id, len, nullptr, &((*pLog)[0])), "Can not link program", false);
	}

	return res != GL_FALSE;
}

int32_t Program::uniformLocation(const std::string& uniformName) const
{
	Context::makeContextCurrent(m->context);
	CHECK_GL_ERROR(auto loc = glGetUniformLocation(m->id, (const GLchar*)uniformName.c_str()), "Can not get uniform location", -1);
	return static_cast<int32_t>(loc);
}

void Program::setUniform(const int32_t location, const float value)
{
	Context::makeContextCurrent(m->context);
	Program::useProgram(shared_from_this());
	CHECK_GL_ERROR(glUniform1f((GLint)location, value), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const int32_t value)
{
	Context::makeContextCurrent(m->context);
	Program::useProgram(shared_from_this());
	CHECK_GL_ERROR(glUniform1i((GLint)location, value), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const uint32_t value)
{
	Context::makeContextCurrent(m->context);
	Program::useProgram(shared_from_this());
	CHECK_GL_ERROR(glUniform1ui((GLint)location, value), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::vec2& value)
{
	Context::makeContextCurrent(m->context);
	Program::useProgram(shared_from_this());
	CHECK_GL_ERROR(glUniform2fv((GLint)location, 1, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::ivec2& value)
{
	Context::makeContextCurrent(m->context);
	Program::useProgram(shared_from_this());
	CHECK_GL_ERROR(glUniform2iv((GLint)location, 1, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::uvec2& value)
{
	Context::makeContextCurrent(m->context);
	Program::useProgram(shared_from_this());
	CHECK_GL_ERROR(glUniform2uiv((GLint)location, 1, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::vec3& value)
{
	Context::makeContextCurrent(m->context);
	Program::useProgram(shared_from_this());
	CHECK_GL_ERROR(glUniform3fv((GLint)location, 1, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::ivec3& value)
{
	Context::makeContextCurrent(m->context);
	Program::useProgram(shared_from_this());
	CHECK_GL_ERROR(glUniform3iv((GLint)location, 1, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::uvec3& value)
{
	Context::makeContextCurrent(m->context);
	Program::useProgram(shared_from_this());
	CHECK_GL_ERROR(glUniform3uiv((GLint)location, 1, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::vec4& value)
{
	Context::makeContextCurrent(m->context);
	Program::useProgram(shared_from_this());
	CHECK_GL_ERROR(glUniform4fv((GLint)location, 1, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::ivec4& value)
{
	Context::makeContextCurrent(m->context);
	Program::useProgram(shared_from_this());
	CHECK_GL_ERROR(glUniform4iv((GLint)location, 1, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::uvec4& value)
{
	Context::makeContextCurrent(m->context);
	Program::useProgram(shared_from_this());
	CHECK_GL_ERROR(glUniform4uiv((GLint)location, 1, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::mat2& value, const bool transpose)
{
	Context::makeContextCurrent(m->context);
	Program::useProgram(shared_from_this());
	CHECK_GL_ERROR(glUniformMatrix2fv((GLint)location, 1, transpose, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::mat3& value, const bool transpose)
{
	Context::makeContextCurrent(m->context);
	Program::useProgram(shared_from_this());
	CHECK_GL_ERROR(glUniformMatrix3fv((GLint)location, 1, transpose, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::mat4& value, const bool transpose)
{
	Context::makeContextCurrent(m->context);
	Program::useProgram(shared_from_this());
	CHECK_GL_ERROR(glUniformMatrix4fv((GLint)location, 1, transpose, glm::value_ptr(value)), "Can not set uniform");
}

Program::Program(ContextPtr context) :
	m(new ProgramPrivate(context))
{
	Context::makeContextCurrent(m->context);
	CHECK_GL_ERROR(auto id = glCreateProgram(), "Can not create program");

	m->id = id;
}

void Program::useProgram(const ProgramPtr& program)
{
	auto cur = ProgramPrivate::currentProgram.lock();
	if (cur == program)
		return;

	CHECK_GL_ERROR(glUseProgram(program->m->id), "Can not use program");
	ProgramPrivate::currentProgram = program;
}

}
