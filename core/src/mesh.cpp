#include <core/mesh.h>

#include "meshprivate.h"

namespace simplex
{
namespace core
{

Mesh::Mesh(const std::shared_ptr<const utils::Mesh>& mesh)
    : m_(std::make_unique<MeshPrivate>())
{
    setMesh(mesh);
}

Mesh::~Mesh() = default;

std::shared_ptr<const utils::Mesh> Mesh::mesh() const
{
    return m_->mesh();
}

void Mesh::setMesh(const std::shared_ptr<const utils::Mesh>& value)
{
    m_->mesh() = value;
    m_->onChanged();
}


}
}
