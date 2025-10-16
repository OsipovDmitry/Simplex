#include <utils/glm/gtc/matrix_inverse.hpp>

#include <core/uniform.h>

#include "mvpstateset.h"

namespace simplex
{
namespace graphics_glfw
{

MVPStateSet::MVPStateSet()
    : StateSet()
{
}

MVPStateSet::~MVPStateSet() = default;

void MVPStateSet::setModelMatrix(const glm::mat4x4 &mm)
{
    const auto nm = glm::mat3x3(glm::inverseTranspose(mm));

    getOrCreateUniform(core::UniformID::ModelMatrix) = core::makeUniform(mm);
    getOrCreateUniform(core::UniformID::NormalMatrix) = core::makeUniform(nm);
    getOrCreateUniform(core::UniformID::NormalMatrixInverse) = core::makeUniform(glm::inverse(nm));
    getOrCreateUniform(core::UniformID::ModelPosition) = core::makeUniform(glm::vec3(mm * glm::vec4(0.f, 0.f, 0.f, 1.f)));
    getOrCreateUniform(core::UniformID::ModelXDirection) = core::makeUniform(glm::vec3(mm * glm::vec4(1.f, 0.f, 0.f, 0.f)));
    getOrCreateUniform(core::UniformID::ModelYDirection) = core::makeUniform(glm::vec3(mm * glm::vec4(0.f, 1.f, 0.f, 0.f)));
    getOrCreateUniform(core::UniformID::ModelZDirection) = core::makeUniform(glm::vec3(mm * glm::vec4(0.f, 0.f, 1.f, 0.f)));
}

}
}
