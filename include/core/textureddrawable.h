#ifndef TEXTUREDDRAWABLE_H
#define TEXTUREDDRAWABLE_H

#include <core/drawablebase.h>

namespace simplex
{
namespace core
{

class TexturedDrawablePrivate;
class CORE_SHARED_EXPORT TexturedDrawable : public DrawableBase
{
    PIMPL(TexturedDrawable)

public:
    TexturedDrawable(std::shared_ptr<IGraphicsRenderer::VertexArray>);
    ~TexturedDrawable() override;

    std::shared_ptr<const IGraphicsRenderer::Texture> texture() const;
    void setTexture(std::shared_ptr<const IGraphicsRenderer::Texture>);

    void setupUniform(const IGraphicsRenderer::RenderProgram::UniformInfo&) override;

};

}
}

#endif // TEXTUREDDRAWABLE_H
