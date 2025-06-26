#ifndef CORE_BACKGROUNDDRAWABLE_H
#define CORE_BACKGROUNDDRAWABLE_H

#include <utils/forwarddecl.h>
#include <utils/glm/vec3.hpp>

#include <core/drawable.h>

namespace simplex
{
namespace core
{

class BackgroundDrawable : public Drawable
{
public:
    BackgroundDrawable(const std::shared_ptr<graphics::IVertexArray>&);
    ~BackgroundDrawable() override;

    graphics::PConstTexture colorMap() const;
    void setColorMap(const graphics::PConstTexture&);

    const glm::vec3 &color() const;
    void setColor(const glm::vec3&);

    float roughness() const;
    void setRoughness(float);

    static const DrawableComponentSet &componentSet();

};

}
}

#endif // CORE_BACKGROUNDDRAWABLE_H
