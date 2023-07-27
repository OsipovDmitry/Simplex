#include <utils/logger.h>

#include <core/standarddrawable.h>

#include "standarddrawableprivate.h"

namespace simplex
{
namespace core
{

const std::string s_baseColorUniformName = "u_baseColor";
const std::string s_metalnessUniformName = "u_metalness";
const std::string s_roughnessUniformName = "u_roughness";
const std::string s_baseColorMapUniformName = "u_baseColorMap";
const std::string s_metalnessMapUniformName = "u_metalnessMap";
const std::string s_roughnessMapUniformName = "u_roughnessMap";
const std::string s_normalMapUniformName = "u_normalMap";

StandardDrawable::~StandardDrawable()
{
}

const glm::vec4 &StandardDrawable::baseColor() const
{
    return uniform_cast<glm::vec4>(uniform(m().baseColorUniformLocation()))->data();
}

void StandardDrawable::setBaseColor(const glm::vec4 &value)
{
    uniform_cast<glm::vec4>(uniform(m().baseColorUniformLocation()))->data() = value;
}

float StandardDrawable::metalness() const
{
    return uniform_cast<float>(uniform(m().metalnessUniformLocation()))->data();
}

void StandardDrawable::setMetalness(float value) const
{
    uniform_cast<float>(uniform(m().metalnessUniformLocation()))->data() = value;
}

float StandardDrawable::roughness() const
{
    return uniform_cast<float>(uniform(m().roughnessUniformLocation()))->data();
}

void StandardDrawable::setRoughness(float value) const
{
    uniform_cast<float>(uniform(m().roughnessUniformLocation()))->data() = value;
}

std::shared_ptr<const graphics::ITexture> StandardDrawable::baseColorMap() const
{
    auto loc = m().baseColorMapUniformLocation();
    return (loc != -1) ? uniform_cast<std::shared_ptr<graphics::ITexture>>(uniform(loc))->data() : nullptr;
}

std::shared_ptr<const graphics::ITexture> StandardDrawable::metalnessMap() const
{
    auto loc = m().metalnessMapUniformLocation();
    return (loc != -1) ? uniform_cast<std::shared_ptr<graphics::ITexture>>(uniform(loc))->data() : nullptr;
}

std::shared_ptr<const graphics::ITexture> StandardDrawable::roughnessMap() const
{
    auto loc = m().roughnessMapUniformLocation();
    return (loc != -1) ? uniform_cast<std::shared_ptr<graphics::ITexture>>(uniform(loc))->data() : nullptr;
}

std::shared_ptr<const graphics::ITexture> StandardDrawable::normalMap() const
{
    auto loc = m().normalMapUniformLocation();
    return (loc != -1) ? uniform_cast<std::shared_ptr<graphics::ITexture>>(uniform(loc))->data() : nullptr;
}

StandardDrawable::StandardDrawable(std::shared_ptr<graphics::IRenderProgram> renderProgram,
                                   std::shared_ptr<graphics::IVertexArray> vao,
                                   const glm::vec4& baseColor,
                                   float metalness,
                                   float roughness,
                                   std::shared_ptr<const graphics::ITexture> baseColorMap,
                                   std::shared_ptr<const graphics::ITexture> metalnessMap,
                                   std::shared_ptr<const graphics::ITexture> roughnessMap,
                                   std::shared_ptr<const graphics::ITexture> normalMap)
    : DrawableBase(std::make_unique<StandardDrawablePrivate>(renderProgram, vao))
{
    auto &mPrivate = m();

    if (auto location = renderProgram->uniformLocationByName(s_baseColorUniformName); location != -1)
    {
        addUniform(location, std::make_shared<Uniform<glm::vec4>>(baseColor));
        mPrivate.baseColorUniformLocation() = location;
    }

    if (auto location = renderProgram->uniformLocationByName(s_metalnessUniformName); location != -1)
    {
        addUniform(location, std::make_shared<Uniform<float>>(metalness));
        mPrivate.metalnessUniformLocation() = location;
    }

    if (auto location = renderProgram->uniformLocationByName(s_roughnessUniformName); location != -1)
    {
        addUniform(location, std::make_shared<Uniform<float>>(roughness));
        mPrivate.roughnessUniformLocation() = location;
    }

    if (auto location = renderProgram->uniformLocationByName(s_baseColorMapUniformName); location != -1)
    {
        addUniform(location, std::make_shared<Uniform<std::shared_ptr<const graphics::ITexture>>>(baseColorMap));
        mPrivate.baseColorMapUniformLocation() = location;
    }

    if (auto location = renderProgram->uniformLocationByName(s_metalnessMapUniformName); location != -1)
    {
        addUniform(location, std::make_shared<Uniform<std::shared_ptr<const graphics::ITexture>>>(metalnessMap));
        mPrivate.metalnessMapUniformLocation() = location;
    }

    if (auto location = renderProgram->uniformLocationByName(s_roughnessMapUniformName); location != -1)
    {
        addUniform(location, std::make_shared<Uniform<std::shared_ptr<const graphics::ITexture>>>(roughnessMap));
        mPrivate.roughnessMapUniformLocation() = location;
    }

    if (auto location = renderProgram->uniformLocationByName(s_normalMapUniformName); location != -1)
    {
        addUniform(location, std::make_shared<Uniform<std::shared_ptr<const graphics::ITexture>>>(normalMap));
        mPrivate.normalMapUniformLocation() = location;
    }
}

}
}
