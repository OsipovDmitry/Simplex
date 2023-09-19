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
    StandardDrawable(std::shared_ptr<graphics::IVertexArray>);
    ~StandardDrawable() override;

    bool isTransparent() const override;

    const glm::vec4 &baseColor() const;
    void setBaseColor(const glm::vec4&);

    float metalness() const;
    void setMetalness(float);

    float roughness() const;
    void setRoughness(float);

    graphics::PConstTexture baseColorMap() const;
    void setBaseColorMap(const graphics::PConstTexture&);

    graphics::PConstTexture metalnessMap() const;
    void setMetalnessMap(const graphics::PConstTexture&);

    graphics::PConstTexture roughnessMap() const;
    void setRoughnessMap(const graphics::PConstTexture&);

    graphics::PConstTexture normalMap() const;
    void setNormalMap(const graphics::PConstTexture&);

};

}
}

#endif // CORE_STANDARDDRAWABLE_H
