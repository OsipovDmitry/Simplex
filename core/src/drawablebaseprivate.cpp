#include "drawablebaseprivate.h"

namespace simplex
{
namespace core
{

DrawableBasePrivate::DrawableBasePrivate(std::shared_ptr<graphics::IRenderProgram> rp, std::shared_ptr<graphics::IVertexArray> vao)
    : m_renderProgram(rp)
    , m_vertexArray(vao)
{}

DrawableBasePrivate::~DrawableBasePrivate()
{}

std::shared_ptr<graphics::IRenderProgram> &DrawableBasePrivate::renderProgram()
{
    return m_renderProgram;
}

std::shared_ptr<graphics::IVertexArray> &DrawableBasePrivate::vertexArray()
{
    return m_vertexArray;
}

std::unordered_map<int32_t, std::shared_ptr<AbstractUniform>> &DrawableBasePrivate::uniforms()
{
    return m_uniforms;
}



}
}
