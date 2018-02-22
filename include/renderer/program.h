#ifndef PROGRAM_H
#define PROGRAM_H

#include <memory>
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

class ProgramPrivate;
class RENDERERSHARED_EXPORT Program : public std::enable_shared_from_this<Program> {
public:
	~Program();

	int id() const;

	void detachShader(ShaderType type);
	void attachShader(ShaderPtr shader);

	bool link(std::string *pLog = nullptr);

	int32_t uniformLocation(const std::string& uniformName) const;

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

private:
	Program(ContextPtr context);

	static void useProgram(const ProgramPtr& program);

	ProgramPrivate *m;

	friend class Context;
};

}

#endif // PROGRAM_H
