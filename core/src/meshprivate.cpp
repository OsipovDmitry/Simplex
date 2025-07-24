#include <core/scene.h>

#include "meshprivate.h"
#include "sceneprivate.h"


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

std::unordered_map<std::shared_ptr<Scene>, std::shared_ptr<MeshHandler>>& MeshPrivate::handles()
{
    return m_handles;
}

void MeshPrivate::onChanged()
{
    for (auto& handle : m_handles)
        handle.first->m().onMeshChanged(handle.second->mesh().lock(), handle.second->ID());
}

}
}
