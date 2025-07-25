#ifndef CORE_DRAWABLEPRIVATE_H
#define CORE_DRAWABLEPRIVATE_H

#include <memory>
#include <string>
#include <unordered_map>

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
    DrawablePrivate(const std::shared_ptr<graphics::IVertexArray>&);
    DrawablePrivate(const std::shared_ptr<const Mesh>&, const std::shared_ptr<const Material>& material);
    ~DrawablePrivate() override;

    std::shared_ptr<const Mesh> &mesh();

    std::shared_ptr<const Material> &material();

    std::shared_ptr<graphics::IVertexArray> &vertexArray();

    bool &isDoubleSided();

    std::unordered_map<std::shared_ptr<SceneData>, std::shared_ptr<DrawableHandler>>& handles();
    void onChanged();

protected:
    std::shared_ptr<const Mesh> m_mesh;
    std::shared_ptr<const Material> m_material;
    std::shared_ptr<graphics::IVertexArray> m_vertexArray;
    bool m_isDoubleSided;
    std::unordered_map<std::shared_ptr<SceneData>, std::shared_ptr<DrawableHandler>> m_handles;

};

}
}

#endif // CORE_DRAWABLEPRIVATE_H
