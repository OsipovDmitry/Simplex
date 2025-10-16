#ifndef CORE_DRAWABLEPRIVATE_H
#define CORE_DRAWABLEPRIVATE_H

#include <memory>
#include <string>

#include <utils/boundingbox.h>
#include <utils/idgenerator.h>

#include <core/forwarddecl.h>

#include "statesetprivate.h"

namespace simplex
{
namespace core
{

class DrawableHandler;
class SceneData;

class DrawablePrivate : public StateSetPrivate
{
public:
    DrawablePrivate(const std::shared_ptr<const Mesh>&, const std::shared_ptr<const Material>& material);
    ~DrawablePrivate() override;

    std::shared_ptr<const Mesh> &mesh();
    std::shared_ptr<const Material> &material();

    std::set<std::shared_ptr<DrawableHandler>>& handlers();
    void onChanged();

    //tmp
    DrawablePrivate(const std::shared_ptr<graphics::VAOMesh>&);
    std::shared_ptr<graphics::VAOMesh>& vertexArray();
    bool& isDoubleSided();

protected:
    std::shared_ptr<const Mesh> m_mesh;
    std::shared_ptr<const Material> m_material;
    std::set<std::shared_ptr<DrawableHandler>> m_handlers;

    //tmp
    std::shared_ptr<graphics::VAOMesh> m_VAOMesh;
    bool m_isDoubleSided;
};

}
}

#endif // CORE_DRAWABLEPRIVATE_H
