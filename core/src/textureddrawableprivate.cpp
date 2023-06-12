#include "textureddrawableprivate.h"

namespace simplex
{
namespace core
{

std::shared_ptr<IGraphicsRenderer::RenderProgram> TexturedDrawablePrivate::s_texturedRenderProgram;

TexturedDrawablePrivate::TexturedDrawablePrivate(std::shared_ptr<IGraphicsRenderer::VertexArray> vao)
    : DrawableBasePrivate(s_texturedRenderProgram, vao)
{
}

std::shared_ptr<const IGraphicsRenderer::Texture> &TexturedDrawablePrivate::texture()
{
    return m_texture;
}

std::shared_ptr<IGraphicsRenderer::RenderProgram> &TexturedDrawablePrivate::texturedRenderProgram()
{
    return s_texturedRenderProgram;
}

}
}
