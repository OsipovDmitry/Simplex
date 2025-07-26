#ifndef CORE_PBRDRAWABLE_H
#define CORE_PBRDRAWABLE_H

#include <utils/glm/vec3.hpp>
#include <utils/glm/vec4.hpp>

#include <core/visualdrawable.h>

namespace simplex
{
namespace core
{

class CORE_SHARED_EXPORT PBRDrawable : public VisualDrawable
{
public:
    PBRDrawable(const std::shared_ptr<graphics::IVertexArray>&, const utils::BoundingBox&);
    PBRDrawable(const std::shared_ptr<Mesh>&, const std::shared_ptr<Material>&, const utils::BoundingBox&);
    ~PBRDrawable() override;

    void setLighted(bool);
    void setShadowed(bool);
    void setShadowCasted(bool);

    const glm::u32vec3 &ORMSwizzleMask() const;
    void setORMSwizzleMask(const glm::u32vec3&);

    const glm::vec4 &baseColor() const;
    void setBaseColor(const glm::vec4&);

    graphics::PConstTexture baseColorMap() const;
    void setBaseColorMap(const graphics::PConstTexture&);

    const glm::vec3 &emission() const;
    void setEmission(const glm::vec3&);

    graphics::PConstTexture emissionMap() const;
    void setEmissionMap(const graphics::PConstTexture&);

    graphics::PConstTexture occlusionMap() const;
    void setOcclusionMap(const graphics::PConstTexture&);

    float occlusionMapStrength() const;
    void setOcclusionMapStrength(float);

    float roughness() const;
    void setRoughness(float);

    graphics::PConstTexture roughnessMap() const;
    void setRoughnessMap(const graphics::PConstTexture&);

    float metalness() const;
    void setMetalness(float);

    graphics::PConstTexture metalnessMap() const;
    void setMetalnessMap(const graphics::PConstTexture&);

    graphics::PConstTexture normalMap() const;
    void setNormalMap(const graphics::PConstTexture&);

    float normalMapScale() const;
    void setNormalMapScale(float);
};

}
}

#endif // CORE_PBRDRAWABLE_H
