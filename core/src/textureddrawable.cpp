#include <utils/logger.h>

#include <core/textureddrawable.h>

#include "textureddrawableprivate.h"

namespace simplex
{
namespace core
{

TexturedDrawable::TexturedDrawable(std::shared_ptr<IGraphicsRenderer::VertexArray> vao)
    : DrawableBase(new TexturedDrawablePrivate(vao))
{
}

TexturedDrawable::~TexturedDrawable()
{
}

std::shared_ptr<const IGraphicsRenderer::Texture> TexturedDrawable::texture() const
{
    return m().texture();
}

void TexturedDrawable::setTexture(std::shared_ptr<const IGraphicsRenderer::Texture> texture)
{
    m().texture() = texture;
}

void TexturedDrawable::setupUniform(const IGraphicsRenderer::RenderProgram::UniformInfo &uniformInfo)
{
    auto &mPrivate = m();
    auto &renderProgram = mPrivate.renderProgram();
    auto uniformName = renderProgram->uniformNameByIndex(uniformInfo.index);

    if (uniformName == "u_texture")
    {
        renderProgram->setUniform(uniformInfo.location, mPrivate.texture());
    }
    else
    {
        LOG_ERROR << "Undefined uniform name \"" << uniformName << "\" in render program";
    }
}

}
}
