#ifndef CORE_DRAWABLE_H
#define CORE_DRAWABLE_H

#include <memory>

#include <core/igraphicsrenderer.h>

namespace simplex
{
namespace core
{

class Drawable
{
public:
    virtual ~Drawable() = default;

    virtual std::shared_ptr<IGraphicsRenderer::RenderProgram> renderProgram() = 0;
    virtual std::shared_ptr<IGraphicsRenderer::VertexArray> vertexArray() = 0;

};

}
}

#endif // CORE_DRAWABLE_H
