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

#include "forwarddecl.h"
#include "renderer_global.h"
#include "../utils/enumclass.h"
#include "../utils/pimpl.h"
#include "../utils/noncopyble.h"
#include "../utils/customdeleter.h"

namespace renderer {

ENUMCLASS(ProgramUniformType, int32_t,
    Float, Float2, Float3, Float4,
	Int, Int2, Int3, Int4,
	Uint, Uint2, Uint3, Uint4,
	Boolean, Bool2, Bool3, Bool4,
	Mat2, Mat3, Mat4, Mat2x3, Mat2x4, Mat3x2, Mat3x4, Mat4x2, Mat4x3,
	Sampler2D, Sampler3D, SamplerCube, Sampler2DArray,
	Sampler2DShadow, SamplerCubeShadow, Sampler2DArrayShadow,
	IntSampler2D, IntSampler3D, IntSamplerCube, IntSampler2DArray,
    UintSampler2D, UintSampler3D, UintSamplerCube, UintSampler2DArray
)

ENUMCLASS(ProgramAttributeType, int32_t,
	Float = 0, Float2, Float3, Float4,
	Int, Int2, Int3, Int4,
	Uint, Uint2, Uint3, Uint4,
    Mat2, Mat3, Mat4, Mat2x3, Mat2x4, Mat3x2, Mat3x4, Mat4x2, Mat4x3
)

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
class RENDERERSHARED_EXPORT Program {
    PIMPL(Program)
    NONCOPYBLE(Program)
    CUSTOMDELETER(Program)

public:
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

    static ProgramPtr create(ContextPtr context);

private:
	Program(ContextPtr context);
    ~Program();

    ProgramPrivate *m_;
};

}

#endif // PROGRAM_H
