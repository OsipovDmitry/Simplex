#ifndef CORE_STANDARDDRAWABLE_H
#define CORE_STANDARDDRAWABLE_H

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
    ~StandardDrawable() override;

    const glm::vec4 &baseColor() const;
    void setBaseColor(const glm::vec4&);

    float metalness() const;
    void setMetalness(float) const;

    float roughness() const;
    void setRoughness(float) const;

    std::shared_ptr<const graphics::ITexture> baseColorMap() const;
    std::shared_ptr<const graphics::ITexture> metalnessMap() const;
    std::shared_ptr<const graphics::ITexture> roughnessMap() const;
    std::shared_ptr<const graphics::ITexture> normalMap() const;

protected:
    StandardDrawable(std::shared_ptr<graphics::IRenderProgram>,
                     std::shared_ptr<graphics::IVertexArray>,
                     const glm::vec4& baseColor,
                     float metalness,
                     float roughness,
                     std::shared_ptr<const graphics::ITexture> baseColorMap,
                     std::shared_ptr<const graphics::ITexture> metalnessMap,
                     std::shared_ptr<const graphics::ITexture> roughnessMap,
                     std::shared_ptr<const graphics::ITexture> normalMap);

    friend class GraphicsEngine;
};

}
}

#endif // CORE_STANDARDDRAWABLE_H
