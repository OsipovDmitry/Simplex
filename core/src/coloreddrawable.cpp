#include <utils/logger.h>

#include <core/coloreddrawable.h>

#include "coloreddrawableprivate.h"

namespace simplex
{
namespace core
{

ColoredDrawable::ColoredDrawable(std::shared_ptr<IGraphicsRenderer::VertexArray> vao)
    : DrawableBase(new ColoredDrawablePrivate(vao))
{
}

ColoredDrawable::~ColoredDrawable()
{
}

const glm::vec4 &ColoredDrawable::color() const
{
    return m().color();
}

void ColoredDrawable::setColor(const glm::vec4 &value)
{
    m().color() = value;
}

void ColoredDrawable::setupUniform(const IGraphicsRenderer::RenderProgram::UniformInfo &uniformInfo)
{
    auto &mPrivate = m();
    auto &renderProgram = mPrivate.renderProgram();
    auto uniformName = renderProgram->uniformNameByIndex(uniformInfo.index);

    if (uniformName == "u_color")
    {
        renderProgram->setUniform(uniformInfo.location, mPrivate.color());
    }
    else
    {
        LOG_ERROR << "Undefined uniform name \"" << uniformName << "\" in render program";
    }
}

}
}
