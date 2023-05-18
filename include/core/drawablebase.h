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
    DrawableBase(std::shared_ptr<IGraphicsRenderer::RenderProgram>, std::shared_ptr<IGraphicsRenderer::VertexArray>);
    ~DrawableBase() override;

    utils::BoundingBox calculateBoundingBox() override;

    std::shared_ptr<IGraphicsRenderer::RenderProgram> renderProgram() override;
    std::shared_ptr<IGraphicsRenderer::VertexArray> vertexArray() override;

    void setupUniform(const IGraphicsRenderer::RenderProgram::UniformInfo&) override;

protected:
    DrawableBase(DrawableBasePrivate*);

    std::unique_ptr<DrawableBasePrivate> m_;

private:
    void initialize();
};

}
}

#endif // CORE_DRAWABLEBASE_H
