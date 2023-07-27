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
    DrawableBase(std::shared_ptr<graphics::IRenderProgram>, std::shared_ptr<graphics::IVertexArray>);
    ~DrawableBase() override;

    utils::BoundingBox calculateBoundingBox() override;

    std::shared_ptr<graphics::IRenderProgram> renderProgram() override;
    std::shared_ptr<graphics::IVertexArray> vertexArray() override;

    std::shared_ptr<const AbstractUniform> uniform(const graphics::UniformInfo&) const override;

protected:
    DrawableBase(std::unique_ptr<DrawableBasePrivate>);

    void addUniform(int32_t, std::shared_ptr<AbstractUniform>);
    void removeUniform(int32_t);
    std::shared_ptr<AbstractUniform> uniform(int32_t) const;

    std::unique_ptr<DrawableBasePrivate> m_;

private:
    void initialize();
};

}
}

#endif // CORE_DRAWABLEBASE_H
