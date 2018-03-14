#ifndef PROGRAM_H
#define PROGRAM_H

#include <memory>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include "renderer_global.h"

namespace renderer {

class Context;
using ContextPtr = std::shared_ptr<Context>;

class Shader;
using ShaderPtr = std::shared_ptr<Shader>;
enum class ShaderType : int32_t;

class Program;
using ProgramPtr = std::shared_ptr<Program>;

enum class ProgramUniformType : int32_t {
	Float = 0, Float2, Float3, Float4,
	Int, Int2, Int3, Int4,
	Uint, Uint2, Uint3, Uint4,
	Boolean, Bool2, Bool3, Bool4,
	Mat2, Mat3, Mat4, Mat2x3, Mat2x4, Mat3x2, Mat3x4, Mat4x2, Mat4x3,
	Sampler2D, Sampler3D, SamplerCube, Sampler2DArray,
	Sampler2DShadow, SamplerCubeShadow, Sampler2DArrayShadow,
	IntSampler2D, IntSampler3D, IntSamplerCube, IntSampler2DArray,
	UintSampler2D, UintSampler3D, UintSamplerCube, UintSampler2DArray,
	Count
};
template <typename T> constexpr ProgramUniformType castToProgramUniformType(T value) { return static_cast<ProgramUniformType>(value); }
template <typename T> constexpr T castFromProgramUniformType(ProgramUniformType value) { return static_cast<T>(value); }

enum class ProgramAttributeType : int32_t {
	Float = 0, Float2, Float3, Float4,
	Int, Int2, Int3, Int4,
	Uint, Uint2, Uint3, Uint4,
	Mat2, Mat3, Mat4, Mat2x3, Mat2x4, Mat3x2, Mat3x4, Mat4x2, Mat4x3,
	Count
};
template <typename T> constexpr ProgramAttributeType castToProgramAttributeType(T value) { return static_cast<ProgramAttributeType>(value); }
template <typename T> constexpr T castFromProgramAttributeType(ProgramAttributeType value) { return static_cast<T>(value); }


struct ProgramUniformInfo {
	std::string name;
	int32_t size;
	int32_t blockIndex;
	int32_t blockOffset;
	int32_t blockArrayStride;
	int32_t blockMatrixStride;
	bool isRowMajor;
	ProgramUniformType type;
};
using ProgramUniformsInfo = std::vector<ProgramUniformInfo>;

struct ProgramUniformBlockInfo {
	std::string name;
	int32_t size;
	std::vector<int32_t> uniformIndices;
};

struct ProgramAttributeInfo {
	std::string name;
	int32_t size;
	ProgramAttributeType type;
};

class ProgramPrivate;
class RENDERERSHARED_EXPORT Program : public std::enable_shared_from_this<Program> {
public:
	~Program();

	void detachShader(ShaderType type);
	void attachShader(ShaderPtr shader);

	bool link(std::string *pLog = nullptr);

	int32_t uniformLocationByName(const std::string& uniformName) const;
	uint32_t uniformBlockIndexByName(const std::string& uniformBlockName) const;
	int32_t attributeLocationByName(const std::string& attributeName) const;

	ProgramUniformsInfo uniformsInfo() const; // быстрее, чем по очереди каждую
	int32_t uniformsCount() const;
	ProgramUniformInfo uniformInfoByIndex(int32_t index) const;

	int32_t uniformBlocksCount() const;
	ProgramUniformBlockInfo uniformBlockInfoByIndex(int32_t index) const;

	int32_t attributesCount() const;
	ProgramAttributeInfo attributeInfoByIndex(int32_t index) const;

	void setUniform(const int32_t location, const float value);
	void setUniform(const int32_t location, const int32_t value);
	void setUniform(const int32_t location, const uint32_t value);
	void setUniform(const int32_t location, const glm::vec2& value);
	void setUniform(const int32_t location, const glm::ivec2& value);
	void setUniform(const int32_t location, const glm::uvec2& value);
	void setUniform(const int32_t location, const glm::vec3& value);
	void setUniform(const int32_t location, const glm::ivec3& value);
	void setUniform(const int32_t location, const glm::uvec3& value);
	void setUniform(const int32_t location, const glm::vec4& value);
	void setUniform(const int32_t location, const glm::ivec4& value);
	void setUniform(const int32_t location, const glm::uvec4& value);
	void setUniform(const int32_t location, const glm::mat2& value, const bool transpose = false);
	void setUniform(const int32_t location, const glm::mat3& value, const bool transpose = false);
	void setUniform(const int32_t location, const glm::mat4& value, const bool transpose = false);
	void setUniform(const int32_t location, const glm::mat2x3& value, const bool transpose = false);
	void setUniform(const int32_t location, const glm::mat2x4& value, const bool transpose = false);
	void setUniform(const int32_t location, const glm::mat3x2& value, const bool transpose = false);
	void setUniform(const int32_t location, const glm::mat3x4& value, const bool transpose = false);
	void setUniform(const int32_t location, const glm::mat4x2& value, const bool transpose = false);
	void setUniform(const int32_t location, const glm::mat4x3& value, const bool transpose = false);
	void setUnifromBlockBindingPoint(const uint32_t uniformBlockIndex, uint32_t bindingPoint);

private:
	Program(ContextPtr context);
	Program(ContextPtr context, ProgramPtr sharedProgram);

	ProgramPrivate *m;

	friend class Context;
	friend class ContextPrivate;
};

}

#endif // PROGRAM_H
