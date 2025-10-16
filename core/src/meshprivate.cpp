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

std::shared_ptr<utils::Mesh>& MeshPrivate::mesh()
{
    return m_mesh;
}

utils::BoundingBox& MeshPrivate::boundingBox()
{
    return m_boundingBox;
}

std::set<std::shared_ptr<MeshHandler>>& MeshPrivate::handlers()
{
    return m_handlers;
}

void MeshPrivate::onChanged()
{
    for (auto& handler : m_handlers)
        if (auto sceneData = handler->sceneData().lock())
            sceneData->onMeshChanged(handler->ID(), m_mesh, m_boundingBox);
}

}
}
