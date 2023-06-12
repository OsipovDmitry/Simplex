#ifndef TEXTUREDDRAWABLEPRIVATE_H
#define TEXTUREDDRAWABLEPRIVATE_H

#include "drawablebaseprivate.h"

namespace simplex
{
namespace core
{

class TexturedDrawablePrivate : public DrawableBasePrivate
{
public:
    TexturedDrawablePrivate(std::shared_ptr<IGraphicsRenderer::VertexArray>);

    std::shared_ptr<const IGraphicsRenderer::Texture> &texture();

    static std::shared_ptr<IGraphicsRenderer::RenderProgram> &texturedRenderProgram();

protected:
    std::shared_ptr<const IGraphicsRenderer::Texture> m_texture;

    static std::shared_ptr<IGraphicsRenderer::RenderProgram> s_texturedRenderProgram;

};

}
}

#endif // TEXTUREDDRAWABLEPRIVATE_H
