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

void MVPStateSet::setViewportSize(const glm::uvec2 &value)
{
    getOrCreateUniform(core::UniformId::ViewportSize) = core::makeUniform(value);
}

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

    glm::mat4x4 vm(1.f);
    if (auto vmUni = core::uniform_cast<glm::mat4x4>(uniform(core::UniformId::ViewMatrix)); vmUni)
        vm = vmUni->data();

    glm::mat4x4 pm(1.f);
    if (auto pmUni = core::uniform_cast<glm::mat4x4>(uniform(core::UniformId::ProjectionMatrix)); pmUni)
        pm = pmUni->data();

    setModelViewMatrix(mm, vm);
    setModelViewProjectionMatrix(mm, vm, pm);
}

void MVPStateSet::setViewMatrix(const glm::mat4 &vm)
{
    const auto vmInv = glm::inverse(vm);

    getOrCreateUniform(core::UniformId::ViewMatrix) = core::makeUniform(vm);
    getOrCreateUniform(core::UniformId::ViewMatrixInverse) = core::makeUniform(vmInv);
    getOrCreateUniform(core::UniformId::ViewPosition) = core::makeUniform(glm::vec3(vmInv * glm::vec4(0.f, 0.f, 0.f, 1.f)));
    getOrCreateUniform(core::UniformId::ViewXDirection) = core::makeUniform(glm::vec3(vmInv * glm::vec4(1.f, 0.f, 0.f, 0.f)));
    getOrCreateUniform(core::UniformId::ViewYDirection) = core::makeUniform(glm::vec3(vmInv * glm::vec4(0.f, 1.f, 0.f, 0.f)));
    getOrCreateUniform(core::UniformId::ViewZDirection) = core::makeUniform(glm::vec3(vmInv * glm::vec4(0.f, 0.f, 1.f, 0.f)));

    glm::mat4x4 mm(1.f);
    if (auto mmUni = core::uniform_cast<glm::mat4x4>(uniform(core::UniformId::ModelMatrix)); mmUni)
        mm = mmUni->data();

    glm::mat4x4 pm(1.f);
    if (auto pmUni = core::uniform_cast<glm::mat4x4>(uniform(core::UniformId::ProjectionMatrix)); pmUni)
        pm = pmUni->data();

    setModelViewMatrix(mm, vm);
    setViewProjectionMatrix(vm, pm);
    setModelViewProjectionMatrix(mm, vm, pm);
}

void MVPStateSet::setProjectionMatrix(const glm::mat4 &pm)
{
    getOrCreateUniform(core::UniformId::ProjectionMatrix) = core::makeUniform(pm);
    getOrCreateUniform(core::UniformId::ProjectionMatrixInverse) = core::makeUniform(glm::inverse(pm));

    glm::mat4x4 mm(1.f);
    if (auto mmUni = core::uniform_cast<glm::mat4x4>(uniform(core::UniformId::ModelMatrix)); mmUni)
        mm = mmUni->data();

    glm::mat4x4 vm(1.f);
    if (auto vmUni = core::uniform_cast<glm::mat4x4>(uniform(core::UniformId::ViewMatrix)); vmUni)
        vm = vmUni->data();

    setViewProjectionMatrix(vm, pm);
    setModelViewProjectionMatrix(mm, vm, pm);
}

void MVPStateSet::setModelViewMatrix(const glm::mat4x4 &mm, const glm::mat4x4 &vm)
{
    const auto mv = vm * mm;
    getOrCreateUniform(core::UniformId::ModelViewMatrix) = core::makeUniform(mv);
    getOrCreateUniform(core::UniformId::NormalViewMatrix) = core::makeUniform(glm::mat3x3(glm::inverseTranspose(mv)));
}

void MVPStateSet::setViewProjectionMatrix(const glm::mat4x4 &vm, const glm::mat4x4 &pm)
{
    const auto vp = pm * vm;
    getOrCreateUniform(core::UniformId::ViewProjectionMatrix) = core::makeUniform(vp);
    getOrCreateUniform(core::UniformId::ViewProjectionMatrixInverse) = core::makeUniform(glm::inverse(vp));
}

void MVPStateSet::setModelViewProjectionMatrix(const glm::mat4x4 &mm, const glm::mat4x4 &vm, const glm::mat4x4 &pm)
{
    getOrCreateUniform(core::UniformId::ModelViewProjectionMatrix) = core::makeUniform(pm * vm * mm);
}

}
}
