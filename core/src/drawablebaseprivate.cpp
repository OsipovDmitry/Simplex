#include "drawablebaseprivate.h"

namespace simplex
{
namespace core
{

DrawableBasePrivate::DrawableBasePrivate(std::shared_ptr<IGraphicsRenderer::RenderProgram> rp, std::shared_ptr<IGraphicsRenderer::VertexArray> vao)
    : m_renderProgram(rp)
    , m_vertexArray(vao)
{}

DrawableBasePrivate::~DrawableBasePrivate()
{}

std::shared_ptr<IGraphicsRenderer::RenderProgram> &DrawableBasePrivate::renderProgram()
{
    return m_renderProgram;
}

std::shared_ptr<IGraphicsRenderer::VertexArray> &DrawableBasePrivate::vertexArray()
{
    return m_vertexArray;
}



}
}
