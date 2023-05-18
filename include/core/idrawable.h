#ifndef CORE_IDRAWABLE_H
#define CORE_IDRAWABLE_H

#include <memory>

#include <utils/forwarddecl.h>

#include <core/igraphicsrenderer.h>

namespace simplex
{
namespace core
{

class IDrawable
{
public:
    virtual ~IDrawable() = default;

    virtual utils::BoundingBox calculateBoundingBox() = 0;

    virtual std::shared_ptr<IGraphicsRenderer::RenderProgram> renderProgram() = 0;
    virtual std::shared_ptr<IGraphicsRenderer::VertexArray> vertexArray() = 0;

    virtual void setupUniform(const IGraphicsRenderer::RenderProgram::UniformInfo&) = 0;
};

}
}

#endif // CORE_IDRAWABLE_H
