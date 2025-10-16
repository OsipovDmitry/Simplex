#include <core/mesh.h>

#include "meshprivate.h"

namespace simplex
{
namespace core
{

Mesh::Mesh()
    : m_(std::make_unique<MeshPrivate>())
{}

Mesh::Mesh(const std::shared_ptr<utils::Mesh>& m, const utils::BoundingBox& bb)
    : Mesh()
{
    setMesh(m, bb);
}

Mesh::~Mesh() = default;

std::shared_ptr<const utils::Mesh> Mesh::mesh() const
{
    return m_->mesh();
}

const utils::BoundingBox& Mesh::boundingBox() const
{
    return m_->boundingBox();
}

void Mesh::setMesh(const std::shared_ptr<utils::Mesh>& m, const utils::BoundingBox& bb)
{
    m_->mesh() = m;
    m_->boundingBox() = bb;
    m_->onChanged();
}


}
}
