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

    getOrCreateUniform(core::UniformId::ModelMatrix) = core::makeUniform(mm);
    getOrCreateUniform(core::UniformId::NormalMatrix) = core::makeUniform(nm);
    getOrCreateUniform(core::UniformId::NormalMatrixInverse) = core::makeUniform(glm::inverse(nm));
    getOrCreateUniform(core::UniformId::ModelPosition) = core::makeUniform(glm::vec3(mm * glm::vec4(0.f, 0.f, 0.f, 1.f)));
    getOrCreateUniform(core::UniformId::ModelXDirection) = core::makeUniform(glm::vec3(mm * glm::vec4(1.f, 0.f, 0.f, 0.f)));
    getOrCreateUniform(core::UniformId::ModelYDirection) = core::makeUniform(glm::vec3(mm * glm::vec4(0.f, 1.f, 0.f, 0.f)));
    getOrCreateUniform(core::UniformId::ModelZDirection) = core::makeUniform(glm::vec3(mm * glm::vec4(0.f, 0.f, 1.f, 0.f)));
}

}
}
