#ifndef CORE_BACKGROUNDDRAWABLE_H
#define CORE_BACKGROUNDDRAWABLE_H

#include <utils/forwarddecl.h>
#include <utils/pimpl.h>
#include <utils/glm/vec3.hpp>

#include <core/drawable.h>

namespace simplex
{
namespace core
{

class BackgroundDrawablePrivate;
class CORE_SHARED_EXPORT BackgroundDrawable : public Drawable
{
    PIMPL(BackgroundDrawable)
public:
    BackgroundDrawable(const std::shared_ptr<graphics::IVertexArray>&);
    ~BackgroundDrawable() override;

    DrawableAlphaMode alphaMode() const override;

    graphics::PConstTexture colorMap() const;
    void setColorMap(const graphics::PConstTexture&);

    const glm::vec3 &baseColor() const;
    void setBaseColor(const glm::vec3&);

    float roughness() const;
    void setRoughness(float);

    static graphics::BackgroundComponentsSet backgroundComponentsSet(const std::shared_ptr<const Drawable>&);
};

}
}
#endif // CORE_BACKGROUNDDRAWABLE_H
