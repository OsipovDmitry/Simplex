#ifndef CORE_COLOREDDRAWABLE_H
#define CORE_COLOREDDRAWABLE_H

#include <core/drawablebase.h>

namespace simplex
{
namespace core
{

class ColoredDrawablePrivate;
class CORE_SHARED_EXPORT ColoredDrawable : public DrawableBase
{
    PIMPL(ColoredDrawable)

public:
    ColoredDrawable(std::shared_ptr<IGraphicsRenderer::VertexArray>);
    ~ColoredDrawable() override;

    const glm::vec4 &color() const;
    void setColor(const glm::vec4&);

    void setupUniform(const IGraphicsRenderer::RenderProgram::UniformInfo&) override;

};

}
}

#endif // CORE_COLOREDDRAWABLE_H
