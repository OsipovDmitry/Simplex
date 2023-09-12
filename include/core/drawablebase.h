#ifndef CORE_DRAWABLEBASE_H
#define CORE_DRAWABLEBASE_H

#include <utils/noncopyble.h>
#include <utils/pimpl.h>

#include <core/coreglobal.h>
#include <core/idrawable.h>

namespace simplex
{
namespace core
{

class DrawableBasePrivate;
class CORE_SHARED_EXPORT DrawableBase : public IDrawable
{
    NONCOPYBLE(DrawableBase)
    PIMPL(DrawableBase)

public:
    explicit DrawableBase(std::shared_ptr<graphics::IVertexArray>);
    ~DrawableBase() override;

    utils::BoundingBox calculateBoundingBox() override;
    bool isTransparent() const override;
    graphics::PBRComponentsSet PBRComponentsSet() const override;

    std::shared_ptr<graphics::IVertexArray> vertexArray() override;

    const PAbstratcUniform uniform(graphics::UniformId) const override;
    PAbstratcUniform &getOrCreateUniform(graphics::UniformId) override;

    const PAbstratcUniform userUniform(const std::string&) const override;
    PAbstratcUniform &getOrCreateUserUniform(const std::string&) override;

    const graphics::PBufferRange SSBO(graphics::SSBOId) const override;

protected:
    DrawableBase(std::unique_ptr<DrawableBasePrivate>);

    std::unique_ptr<DrawableBasePrivate> m_;
};

}
}

#endif // CORE_DRAWABLEBASE_H
