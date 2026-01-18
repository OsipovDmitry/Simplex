#ifndef CORE_BACKGROUNDPRIVATE_H
#define CORE_BACKGROUNDPRIVATE_H

#include <memory>

#include <utils/glm/vec3.hpp>
#include <utils/glm/ext/quaternion_float.hpp>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class SceneData;
class BackgroundHandler;

class BackgroundPrivate
{
public:
    BackgroundPrivate();
    ~BackgroundPrivate();

    std::shared_ptr<MaterialMap> &environmentMap();
    glm::quat& rotation();
    glm::vec3 &environmentColor();
    float &blurPower();

    std::shared_ptr<BackgroundHandler>& handler();
    void onChanged();

private:
    std::shared_ptr<MaterialMap> m_environmentMap;
    glm::quat m_rotation;
    glm::vec3 m_environmentColor;
    float m_blurPower;

    std::shared_ptr<BackgroundHandler> m_handler;
};

}
}

#endif // CORE_BACKGROUNDPRIVATE_H
