#include "coloreddrawableprivate.h"

namespace simplex
{
namespace core
{

std::shared_ptr<IGraphicsRenderer::RenderProgram> ColoredDrawablePrivate::s_coloredRenderProgram;

ColoredDrawablePrivate::ColoredDrawablePrivate(std::shared_ptr<IGraphicsRenderer::VertexArray> vao)
    : DrawableBasePrivate(s_coloredRenderProgram, vao)
    , m_color(0.f, 0.f, 0.f, 1.f)
{
}

glm::vec4 &ColoredDrawablePrivate::color()
{
    return m_color;
}

std::shared_ptr<IGraphicsRenderer::RenderProgram> &ColoredDrawablePrivate::coloredRenderProgram()
{
    return s_coloredRenderProgram;
}

}
}
