#ifndef CORE_DRAWABLEBASE_H
#define CORE_DRAWABLEBASE_H

#include <core/coreglobal.h>
#include <core/idrawable.h>

namespace simplex
{
namespace core
{

class DrawableBasePrivate;
class CORE_SHARED_EXPORT DrawableBase : public IDrawable
{
public:
    DrawableBase(std::shared_ptr<IGraphicsRenderer::RenderProgram>, std::shared_ptr<IGraphicsRenderer::VertexArray>);
    ~DrawableBase() override;

    utils::BoundingBox calculateBoundingBox() override;

    std::shared_ptr<IGraphicsRenderer::RenderProgram> renderProgram() override;
    std::shared_ptr<IGraphicsRenderer::VertexArray> vertexArray() override;

private:
    std::unique_ptr<DrawableBasePrivate> m_;
};

}
}

#endif // CORE_DRAWABLEBASE_H
