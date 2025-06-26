#ifndef CORE_BACKGROUND_H
#define CORE_BACKGROUND_H

#include <memory>

#include <utils/glm/vec3.hpp>
#include <utils/pimpl.h>
#include <utils/noncopyble.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class BackgroundPrivate;
class CORE_SHARED_EXPORT Background
{
    NONCOPYBLE(Background)
    PRIVATE_IMPL(Background)
public:
    Background();
    ~Background();

    graphics::PConstTexture colorMap() const;
    void setColorMap(const graphics::PConstTexture&);

    const glm::vec3 &color() const;
    void setColor(const glm::vec3&);

    float roughness() const;
    void setRoughness(float);

private:
    std::unique_ptr<BackgroundPrivate> m_;

};

}
}

#endif // CORE_BACKGROUND_H
