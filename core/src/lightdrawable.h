#ifndef CORE_LIGHTDRAWABLE_H
#define CORE_LIGHTDRAWABLE_H

#include <utils/glm/mat4x4.hpp>
#include <utils/forwarddecl.h>
#include <utils/enumclass.h>

#include <core/drawable.h>

namespace simplex
{
namespace core
{

class LightDrawable : public Drawable
{
public:
    LightDrawable(const std::shared_ptr<graphics::VAOMesh>&);
    ~LightDrawable() override;

    float dielectricSpecular() const;
    void setDielecticSpecular(float);

    void setShadowDepthMap(const graphics::PConstTexture&);
    void setShadowColorMap(const graphics::PConstTexture&);

    void setShadowViewMatrix(const glm::mat4x4&);
    void setShadowProjectionMatrix(const glm::mat4x4&);

    void setShadowDepthBias(float);

    static const DrawableComponentSet &componentSet();
};

}
}

#endif // CORE_LIGHTDRAWABLE_H
