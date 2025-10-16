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

    std::shared_ptr<MaterialMap> environmentMap();
    std::shared_ptr<const MaterialMap> environmentMap() const;
    void setEnvironmentMap(const std::shared_ptr<MaterialMap>&);

    const glm::vec3& environmentColor() const;
    void setEnvironmentColor(const glm::vec3&) const;

    float blurPower() const;
    void setBlurPower(float);

private:
    std::unique_ptr<BackgroundPrivate> m_;

};

}
}

#endif // CORE_BACKGROUND_H
