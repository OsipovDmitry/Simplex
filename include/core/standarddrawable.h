#ifndef CORE_STANDARDDRAWABLE_H
#define CORE_STANDARDDRAWABLE_H

#include <utils/glm/vec4.hpp>
#include <utils/pimpl.h>

#include <core/drawable.h>

namespace simplex
{
namespace core
{

class StandardDrawablePrivate;
class CORE_SHARED_EXPORT StandardDrawable : public Drawable
{
    PIMPL(StandardDrawable)
public:
    StandardDrawable(const std::shared_ptr<graphics::IVertexArray>&, const utils::BoundingBox&);
    ~StandardDrawable() override;

    const utils::BoundingBox &boundingBox() const override;

    DrawableAlphaMode alphaMode() const override;

    const glm::u32vec4 &ORMSwizzleMask() const;
    void setORMSwizzleMask(const glm::u32vec4&);

    const glm::vec4 &baseColor() const;
    void setBaseColor(const glm::vec4&);

    graphics::PConstTexture baseColorMap() const;
    void setBaseColorMap(const graphics::PConstTexture&);

    float metalness() const;
    void setMetalness(float);

    graphics::PConstTexture metalnessMap() const;
    void setMetalnessMap(const graphics::PConstTexture&);

    float roughness() const;
    void setRoughness(float);

    graphics::PConstTexture roughnessMap() const;
    void setRoughnessMap(const graphics::PConstTexture&);

    graphics::PConstTexture normalMap() const;
    void setNormalMap(const graphics::PConstTexture&);

    float normalMapScale() const;
    void setNormalMapScale(float);

    graphics::PConstTexture occlusionMap() const;
    void setOcclusionMap(const graphics::PConstTexture&);

    float occlusionMapStrength() const;
    void setOcclusionMapStrength(float);

    static graphics::PBRComponentsSet PBRComponentsSet(const std::shared_ptr<const Drawable>&);
};

}
}

#endif // CORE_STANDARDDRAWABLE_H
