#ifndef CORE_BACKGROUNDPRIVATE_H
#define CORE_BACKGROUNDPRIVATE_H

#include <memory>

#include <utils/glm/vec3.hpp>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

/*tmp*/class BackgroundDrawable;

class SceneData;
class BackgroundHandler;

class BackgroundPrivate
{
public:
    BackgroundPrivate();
    ~BackgroundPrivate();

    std::shared_ptr<MaterialMap> &environmentMap();
    glm::vec3 &environmentColor();
    float &blurPower();

    std::pair<std::shared_ptr<SceneData>, std::shared_ptr<BackgroundHandler>>& handler();
    void onChanged();

    //tmp
    std::shared_ptr<BackgroundDrawable>& drawable();

private:
    std::shared_ptr<MaterialMap> m_environmentMap;
    glm::vec3 m_environmentColor;
    float m_blurPower;

    std::pair<std::shared_ptr<SceneData>, std::shared_ptr<BackgroundHandler>> m_handler;

    //tmp
    std::shared_ptr<BackgroundDrawable> m_drawable;

};

}
}

#endif // CORE_BACKGROUNDPRIVATE_H
