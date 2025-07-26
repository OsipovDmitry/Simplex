#include <core/scene.h>

#include "meshprivate.h"
#include "scenedata.h"


namespace simplex
{
namespace core
{

MeshPrivate::MeshPrivate()
{
}

MeshPrivate::~MeshPrivate() = default;

std::shared_ptr<const utils::Mesh>& MeshPrivate::mesh()
{
    return m_mesh;
}

std::unordered_map<std::shared_ptr<SceneData>, std::shared_ptr<MeshHandler>>& MeshPrivate::handlers()
{
    return m_handlers;
}

void MeshPrivate::onChanged()
{
    for (auto& handle : m_handlers)
        handle.first->onMeshChanged(handle.second->mesh().lock(), handle.second->ID());
}

}
}
