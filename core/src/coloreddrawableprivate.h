#ifndef CORE_COLOREDDRAWABLEPRIVATE_H
#define CORE_COLOREDDRAWABLEPRIVATE_H

#include "drawablebaseprivate.h"

namespace simplex
{
namespace core
{

class ColoredDrawablePrivate : public DrawableBasePrivate
{
public:
    ColoredDrawablePrivate(std::shared_ptr<IGraphicsRenderer::VertexArray>);

    glm::vec4 &color();

    static std::shared_ptr<IGraphicsRenderer::RenderProgram> &coloredRenderProgram();

protected:
    glm::vec4 m_color;

    static std::shared_ptr<IGraphicsRenderer::RenderProgram> s_coloredRenderProgram;

};

}
}

#endif // CORE_COLOREDDRAWABLEPRIVATE_H
