#ifndef CORE_MESHPRIVATE_H
#define CORE_MESHPRIVATE_H

#include <utils/forwarddecl.h>

#include <core/forwarddecl.h>
#include <core/material.h>

namespace simplex
{
namespace core
{

class MeshHandler;
class SceneData;

class MeshPrivate
{
public:
    MeshPrivate();
    ~MeshPrivate();

    std::shared_ptr<const utils::Mesh> &mesh();

    std::unordered_map<std::shared_ptr<SceneData>, std::shared_ptr<MeshHandler>> &handlers();
    void onChanged();

private:
    std::shared_ptr<const utils::Mesh> m_mesh;
    std::unordered_map<std::shared_ptr<SceneData>, std::shared_ptr<MeshHandler>> m_handlers;
};

}
}

#endif // CORE_MESHPRIVATE_H
