#ifndef CORE_STANDARDDRAWABLE_H
#define CORE_STANDARDDRAWABLE_H

#include <utils/glm/vec4.hpp>
#include <utils/pimpl.h>

#include <core/drawablebase.h>

namespace simplex
{
namespace core
{

class StandardDrawablePrivate;
class CORE_SHARED_EXPORT StandardDrawable : public DrawableBase
{
    PIMPL(StandardDrawable)
public:
    StandardDrawable(std::shared_ptr<graphics::IVertexArray>,
                     const glm::vec4& baseColor,
                     float metalness,
                     float roughness,
                     graphics::PTexture baseColorMap = nullptr,
                     graphics::PTexture metalnessMap = nullptr,
                     graphics::PTexture roughnessMap = nullptr,
                     graphics::PTexture normalMap = nullptr);
    ~StandardDrawable() override;

    bool isTransparent() const override;
    graphics::PBRComponentsSet PBRComponentsSet() const override;

    const glm::vec4 &baseColor() const;
    void setBaseColor(const glm::vec4&);

    float metalness() const;
    void setMetalness(float);

    float roughness() const;
    void setRoughness(float);

    graphics::PTexture baseColorMap() const;
    graphics::PTexture metalnessMap() const;
    graphics::PTexture roughnessMap() const;
    graphics::PTexture normalMap() const;

};

}
}

#endif // CORE_STANDARDDRAWABLE_H
