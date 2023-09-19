#ifndef CORE_IDRAWABLE_H
#define CORE_IDRAWABLE_H

#include <memory>

#include <utils/forwarddecl.h>
#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class IDrawable
{
public:
    virtual ~IDrawable() = default;

    virtual utils::BoundingBox calculateBoundingBox() = 0;
    virtual bool isTransparent() const = 0;
    virtual graphics::PBRComponentsSet PBRComponentsSet() const = 0;

    virtual std::shared_ptr<const graphics::IVertexArray> vertexArray() const = 0;

    virtual const PAbstratcUniform uniform(graphics::UniformId) const = 0;
    virtual PAbstratcUniform &getOrCreateUniform(graphics::UniformId) = 0;

    virtual const PAbstratcUniform userUniform(const std::string&) const = 0;
    virtual PAbstratcUniform &getOrCreateUserUniform(const std::string&) = 0;

    virtual const graphics::PBufferRange SSBO(graphics::SSBOId) const = 0;

};

}
}

#endif // CORE_IDRAWABLE_H
