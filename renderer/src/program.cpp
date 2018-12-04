#include <array>
#include <algorithm>

#include <glm/gtc/type_ptr.hpp>

#include <renderer/program.h>
#include <renderer/shader.h>
#include <renderer/context.h>

#include "glutils.h"
#include "context_p.h"
#include "shader_p.h"
#include "program_p.h"

namespace renderer {

namespace {

const std::array<GLenum, castFromProgramUniformType(ProgramUniformType::Count)> uniformTypeTable {
	GL_FLOAT, GL_FLOAT_VEC2, GL_FLOAT_VEC3, GL_FLOAT_VEC4,
	GL_INT, GL_INT_VEC2, GL_INT_VEC3, GL_INT_VEC4,
	GL_UNSIGNED_INT, GL_UNSIGNED_INT_VEC2, GL_UNSIGNED_INT_VEC3, GL_UNSIGNED_INT_VEC4,
	GL_BOOL, GL_BOOL_VEC2, GL_BOOL_VEC3, GL_BOOL_VEC4,
	GL_FLOAT_MAT2, GL_FLOAT_MAT3, GL_FLOAT_MAT4, GL_FLOAT_MAT2x3, GL_FLOAT_MAT2x4, GL_FLOAT_MAT3x2, GL_FLOAT_MAT3x4, GL_FLOAT_MAT4x2, GL_FLOAT_MAT4x3,
	GL_SAMPLER_2D, GL_SAMPLER_3D, GL_SAMPLER_CUBE, GL_SAMPLER_2D_ARRAY,
	GL_SAMPLER_2D_SHADOW, GL_SAMPLER_CUBE_SHADOW, GL_SAMPLER_2D_ARRAY_SHADOW,
	GL_INT_SAMPLER_2D, GL_INT_SAMPLER_3D, GL_INT_SAMPLER_CUBE, GL_INT_SAMPLER_2D_ARRAY,
	GL_UNSIGNED_INT_SAMPLER_2D, GL_UNSIGNED_INT_SAMPLER_CUBE, GL_UNSIGNED_INT_SAMPLER_2D_ARRAY,
};

GLenum toUnifromGLType(ProgramUniformType val) {
    return uniformTypeTable[castFromProgramUniformType(val)];
}

ProgramUniformType fromUniformGLType(GLenum val) {
	return castToProgramUniformType(std::find(uniformTypeTable.cbegin(), uniformTypeTable.cend(), (GLenum)val) - uniformTypeTable.cbegin());
}

const std::array<GLenum, castFromProgramAttributeType(ProgramAttributeType::Count)> attributeTypeTable {
	GL_FLOAT, GL_FLOAT_VEC2, GL_FLOAT_VEC3, GL_FLOAT_VEC4,
	GL_INT, GL_INT_VEC2, GL_INT_VEC3, GL_INT_VEC4,
	GL_UNSIGNED_INT, GL_UNSIGNED_INT_VEC2, GL_UNSIGNED_INT_VEC3, GL_UNSIGNED_INT_VEC4,
	GL_FLOAT_MAT2, GL_FLOAT_MAT3, GL_FLOAT_MAT4, GL_FLOAT_MAT2x3, GL_FLOAT_MAT2x4, GL_FLOAT_MAT3x2, GL_FLOAT_MAT3x4, GL_FLOAT_MAT4x2, GL_FLOAT_MAT4x3,
};

GLenum toAttributeGLType(ProgramAttributeType val) {
    return attributeTypeTable[castFromProgramAttributeType(val)];
}

ProgramAttributeType fromAttributeGLType(GLenum val) {
	return castToProgramAttributeType(std::find(attributeTypeTable.cbegin(), attributeTypeTable.cend(), (GLenum)val) - attributeTypeTable.cbegin());
}

}

void Program::detachShader(ShaderType type)
{
    m_->context->m()->bindThisContext();
    auto& p = m_->shaders[castFromShaderType(type)];
	if (p) {
        CHECK_GL_ERROR(glDetachShader(m_->id, p->m()->id), "Can not detach shader");
		p.reset();
	}
}

void Program::attachShader(ShaderPtr shader)
{
    if (m_->context != shader->context()) {
		LOG_CRITICAL("Program and shader are in different contexts");
		return;
	}

    m_->context->m()->bindThisContext();
	auto type = shader->type();
	detachShader(type);
    CHECK_GL_ERROR(glAttachShader(m_->id, shader->m()->id), "Can not attach shader");
    m_->shaders[castFromShaderType(type)] = shader;
}

bool Program::link(std::string* pLog)
{
    m_->context->m()->bindThisContext();
    CHECK_GL_ERROR(glLinkProgram(m_->id), "Can not link program", false);

	GLint res;
    CHECK_GL_ERROR(glGetProgramiv(m_->id, GL_LINK_STATUS, &res), "Can not link program", false);

	if (pLog && (res == GL_FALSE)) {
		GLint len;
        CHECK_GL_ERROR(glGetProgramiv(m_->id, GL_INFO_LOG_LENGTH, &len), "Can not link program", false);
		pLog->resize(len);
        CHECK_GL_ERROR(glGetProgramInfoLog(m_->id, len, nullptr, &((*pLog)[0])), "Can not link program", false);
	}

	return res != GL_FALSE;
}

int32_t Program::uniformLocationByName(const std::string& uniformName) const
{
    m_->context->m()->bindThisContext();
    CHECK_GL_ERROR(auto loc = glGetUniformLocation(m_->id, (const GLchar*)uniformName.c_str()), "Can not get uniform location", -1);
	return static_cast<int32_t>(loc);
}

uint32_t Program::uniformBlockIndexByName(const std::string& uniformBlockName) const
{
    m_->context->m()->bindThisContext();
    CHECK_GL_ERROR(auto idx = glGetUniformBlockIndex(m_->id, (const GLchar*)uniformBlockName.c_str()), "Can not get uniform block index", -1);
	return static_cast<uint32_t>(idx);
}

int32_t Program::attributeLocationByName(const std::string& attributeName) const
{
    m_->context->m()->bindThisContext();
    CHECK_GL_ERROR(auto loc = glGetAttribLocation(m_->id, (const GLchar*)attributeName.c_str()), "Can not get attribute location", -1);
	return static_cast<int32_t>(loc);
}

ProgramUniformsInfo Program::uniformsInfo() const
{
    m_->context->m()->bindThisContext();

	ProgramUniformsInfo info;
	auto numUniforms = uniformsCount();
	if (numUniforms < 0)
		return info;
	info.resize(numUniforms);

	std::vector<GLint> intVec(numUniforms);

	std::vector<GLuint> uniformIndices(numUniforms);
	for (int32_t i = 0; i < numUniforms; ++i)
		uniformIndices[i] = (GLuint)i;

    CHECK_GL_ERROR(glGetActiveUniformsiv(m_->id, numUniforms, uniformIndices.data(), GL_UNIFORM_TYPE, intVec.data()), "Can not get active uniform info", info);
	for (int32_t i = 0; i < numUniforms; ++i)
		info[i].type = fromUniformGLType((GLenum)intVec[i]);

    CHECK_GL_ERROR(glGetActiveUniformsiv(m_->id, numUniforms, uniformIndices.data(), GL_UNIFORM_SIZE, intVec.data()), "Can not get active uniform info", info);
	for (int32_t i = 0; i < numUniforms; ++i)
		info[i].size = intVec[i];

    CHECK_GL_ERROR(glGetActiveUniformsiv(m_->id, numUniforms, uniformIndices.data(), GL_UNIFORM_BLOCK_INDEX, intVec.data()), "Can not get active uniform info", info);
	for (int32_t i = 0; i < numUniforms; ++i)
		info[i].blockIndex = intVec[i];

    CHECK_GL_ERROR(glGetActiveUniformsiv(m_->id, numUniforms, uniformIndices.data(), GL_UNIFORM_OFFSET, intVec.data()), "Can not get active uniform info", info);
	for (int32_t i = 0; i < numUniforms; ++i)
		info[i].blockOffset = intVec[i];

    CHECK_GL_ERROR(glGetActiveUniformsiv(m_->id, numUniforms, uniformIndices.data(), GL_UNIFORM_ARRAY_STRIDE, intVec.data()), "Can not get active uniform info", info);
	for (int32_t i = 0; i < numUniforms; ++i)
		info[i].blockArrayStride = intVec[i];

    CHECK_GL_ERROR(glGetActiveUniformsiv(m_->id, numUniforms, uniformIndices.data(), GL_UNIFORM_MATRIX_STRIDE, intVec.data()), "Can not get active uniform info", info);
	for (int32_t i = 0; i < numUniforms; ++i)
		info[i].blockMatrixStride = intVec[i];

    CHECK_GL_ERROR(glGetActiveUniformsiv(m_->id, numUniforms, uniformIndices.data(), GL_UNIFORM_IS_ROW_MAJOR, intVec.data()), "Can not get active uniform info", info);
	for (int32_t i = 0; i < numUniforms; ++i)
		info[i].isRowMajor = intVec[i] != 0;

    CHECK_GL_ERROR(glGetActiveUniformsiv(m_->id, numUniforms, uniformIndices.data(), GL_UNIFORM_NAME_LENGTH, intVec.data()), "Can not get active uniform info", info);
	for (int32_t i = 0; i < numUniforms; ++i) {
		info[i].name.resize(intVec[i]);
        CHECK_GL_ERROR(glGetActiveUniform(m_->id, i, intVec[i], nullptr, nullptr, nullptr, (GLchar*)(&(info[i].name[0]))), "Can not get active uniform info", info);
		info[i].name.resize(intVec[i]-1);
	}

	return info;
}

int32_t Program::uniformsCount() const
{
    m_->context->m()->bindThisContext();

	GLint numUniforms;
    CHECK_GL_ERROR(glGetProgramiv(m_->id, GL_ACTIVE_UNIFORMS, &numUniforms), "Can not get active uniforms count", -1);
	return numUniforms;
}

ProgramUniformInfo Program::uniformInfoByIndex(int32_t index) const
{
    m_->context->m()->bindThisContext();
	ProgramUniformInfo info;

	GLuint GLindex = index;
	GLint val;

    CHECK_GL_ERROR(glGetActiveUniformsiv(m_->id, 1, &GLindex, GL_UNIFORM_TYPE, &val), "Can not get active uniform info", info);
	info.type = fromUniformGLType((GLenum)val);

    CHECK_GL_ERROR(glGetActiveUniformsiv(m_->id, 1, &GLindex, GL_UNIFORM_SIZE, &val), "Can not get active uniform info", info);
	info.size = val;

    CHECK_GL_ERROR(glGetActiveUniformsiv(m_->id, 1, &GLindex, GL_UNIFORM_BLOCK_INDEX, &val), "Can not get active uniform info", info);
	info.blockIndex = val;

    CHECK_GL_ERROR(glGetActiveUniformsiv(m_->id, 1, &GLindex, GL_UNIFORM_OFFSET, &val), "Can not get active uniform info", info);
	info.blockOffset = val;

    CHECK_GL_ERROR(glGetActiveUniformsiv(m_->id, 1, &GLindex, GL_UNIFORM_ARRAY_STRIDE, &val), "Can not get active uniform info", info);
	info.blockArrayStride = val;

    CHECK_GL_ERROR(glGetActiveUniformsiv(m_->id, 1, &GLindex, GL_UNIFORM_MATRIX_STRIDE, &val), "Can not get active uniform info", info);
	info.blockMatrixStride = val;

    CHECK_GL_ERROR(glGetActiveUniformsiv(m_->id, 1, &GLindex, GL_UNIFORM_IS_ROW_MAJOR, &val), "Can not get active uniform info", info);
	info.isRowMajor = val != 0;

    CHECK_GL_ERROR(glGetActiveUniformsiv(m_->id, 1, &GLindex, GL_UNIFORM_NAME_LENGTH, &val), "Can not get active uniform info", info);
	info.name.resize(val);
    CHECK_GL_ERROR(glGetActiveUniform(m_->id, GLindex, val, nullptr, nullptr, nullptr, (GLchar*)(&(info.name[0]))), "Can not get active uniform info", info);
	info.name.resize(val-1);

	return info;

}

int32_t Program::uniformBlocksCount() const
{
    m_->context->m()->bindThisContext();

	GLint numUniforms;
    CHECK_GL_ERROR(glGetProgramiv(m_->id, GL_ACTIVE_UNIFORM_BLOCKS, &numUniforms), "Can not get active uniform blocks count", -1);
	return numUniforms;
}

ProgramUniformBlockInfo Program::uniformBlockInfoByIndex(int32_t index) const
{
    m_->context->m()->bindThisContext();

	ProgramUniformBlockInfo info;
	GLint val;

    CHECK_GL_ERROR(glGetActiveUniformBlockiv(m_->id, index, GL_UNIFORM_BLOCK_NAME_LENGTH, &val), "Can not get uniform block info", info);
	info.name.resize(val);
    CHECK_GL_ERROR(glGetActiveUniformBlockName(m_->id, index, val, nullptr, (GLchar*)(&(info.name[0]))), "Can not get uniform block info", info);
	info.name.resize(val-1);

    CHECK_GL_ERROR(glGetActiveUniformBlockiv(m_->id, index, GL_UNIFORM_BLOCK_DATA_SIZE, (GLint*)&val), "Can not get uniform block info", info);
	info.size = val;

    CHECK_GL_ERROR(glGetActiveUniformBlockiv(m_->id, index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, (GLint*)&val), "Can not get uniform block info", info);
	info.uniformIndices.resize(val);
    CHECK_GL_ERROR(glGetActiveUniformBlockiv(m_->id, index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, (GLint*)info.uniformIndices.data()), "Can not get uniform block info", info);

	return info;
}

int32_t Program::attributesCount() const
{
    m_->context->m()->bindThisContext();

	GLint numAttributes;
    CHECK_GL_ERROR(glGetProgramiv(m_->id, GL_ACTIVE_ATTRIBUTES, &numAttributes), "Can not get active attributes count", -1);
	return numAttributes;
}

ProgramAttributeInfo Program::attributeInfoByIndex(int32_t index) const
{
    m_->context->m()->bindThisContext();

	ProgramAttributeInfo info;
	GLint nameLength, size;
	GLenum type;

    CHECK_GL_ERROR(glGetProgramiv(m_->id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &nameLength), "Can not get active attribute info", info);
	info.name.resize(nameLength);

    CHECK_GL_ERROR(glGetActiveAttrib(m_->id, index, nameLength, &nameLength, &size, &type, (GLchar*)(&(info.name[0]))), "Can not get active attribute info", info);
	info.name.resize(nameLength);
	info.size = size;
	info.type = fromAttributeGLType(type);

	return info;
}

void Program::setUniform(const int32_t location, const float value)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniform1f((GLint)location, value), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const int32_t value)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniform1i((GLint)location, value), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const uint32_t value)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniform1ui((GLint)location, value), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::vec2& value)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniform2fv((GLint)location, 1, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::ivec2& value)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniform2iv((GLint)location, 1, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::uvec2& value)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniform2uiv((GLint)location, 1, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::vec3& value)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniform3fv((GLint)location, 1, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::ivec3& value)
{
    m_->context->m()->bindThisContext();;
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniform3iv((GLint)location, 1, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::uvec3& value)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniform3uiv((GLint)location, 1, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::vec4& value)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniform4fv((GLint)location, 1, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::ivec4& value)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniform4iv((GLint)location, 1, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::uvec4& value)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniform4uiv((GLint)location, 1, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::mat2& value, const bool transpose)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniformMatrix2fv((GLint)location, 1, transpose, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::mat3& value, const bool transpose)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniformMatrix3fv((GLint)location, 1, transpose, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::mat4& value, const bool transpose)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniformMatrix4fv((GLint)location, 1, transpose, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::mat2x3& value, const bool transpose)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniformMatrix2x3fv((GLint)location, 1, transpose, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::mat2x4& value, const bool transpose)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniformMatrix2x4fv((GLint)location, 1, transpose, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::mat3x2& value, const bool transpose)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniformMatrix3x2fv((GLint)location, 1, transpose, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::mat3x4& value, const bool transpose)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniformMatrix3x4fv((GLint)location, 1, transpose, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::mat4x2& value, const bool transpose)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniformMatrix4x2fv((GLint)location, 1, transpose, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUniform(const int32_t location, const glm::mat4x3& value, const bool transpose)
{
    m_->context->m()->bindThisContext();
    m_->context->m()->bindProgram(this);
	CHECK_GL_ERROR(glUniformMatrix4x3fv((GLint)location, 1, transpose, glm::value_ptr(value)), "Can not set uniform");
}

void Program::setUnifromBlockBindingPoint(const uint32_t uniformBlockIndex, uint32_t bindingPoint)
{
    m_->context->m()->bindThisContext();
    CHECK_GL_ERROR(glUniformBlockBinding(m_->id, uniformBlockIndex, bindingPoint), "Can not set uniform block binding point");
}

ProgramPtr Program::create(ContextPtr context)
{
    return ProgramPtr(new Program(context), ProgramDeleter());
}

Program::Program(ContextPtr context) :
    m_(new ProgramPrivate(context))
{
    m_->context->m()->bindThisContext();
    CHECK_GL_ERROR(m_->id = glCreateProgram(), "Can not create program");
}

Program::~Program()
{
    m_->context->m()->bindThisContext();
    CHECK_GL_ERROR(glDeleteProgram(m_->id), "Can not delete program");
    delete m_;
}

}
