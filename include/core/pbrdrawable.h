#ifndef CORE_PBRDRAWABLE_H
#define CORE_PBRDRAWABLE_H

#include <utils/glm/vec4.hpp>
#include <utils/pimpl.h>

#include <core/drawable.h>

namespace simplex
{
namespace core
{

ENUMCLASS(PBRComponent, uint16_t,
          BaseColor,
          BaseColorMap,
          Metalness,
          MetalnessMap,
          Roughness,
          RoughnessMap,
          NormalMap,
          NormalMapScale,
          OcclusionMap,
          OcclusionMapStrength)

class PBRDrawablePrivate;
class CORE_SHARED_EXPORT PBRDrawable : public Drawable
{
    PIMPL(PBRDrawable)
public:
    PBRDrawable(const std::shared_ptr<graphics::IVertexArray>&, const utils::BoundingBox&);
    ~PBRDrawable() override;

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

    static std::unordered_set<PBRComponent> PBRComponentsSet(const std::shared_ptr<const Drawable>&);

    static graphics::UniformId uniformIdByPBRComponent(PBRComponent);
};

}
}

#endif // CORE_PBRDRAWABLE_H
