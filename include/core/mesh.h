#ifndef CORE_MESH_H
#define CORE_MESH_H

#include <memory>

#include <utils/noncopyble.h>
#include <utils/pimpl.h>
#include <utils/forwarddecl.h>

#include <core/coreglobal.h>

namespace simplex
{
namespace core
{

class MeshPrivate;
class CORE_SHARED_EXPORT Mesh
{
    NONCOPYBLE(Mesh)
    PRIVATE_IMPL(Mesh)
public:
    Mesh();
    Mesh(const std::shared_ptr<utils::Mesh>&, const utils::BoundingBox&);
    ~Mesh();

    std::shared_ptr<const utils::Mesh> mesh() const;
    const utils::BoundingBox& boundingBox() const;

    void setMesh(const std::shared_ptr<utils::Mesh>&, const utils::BoundingBox&);

private:
    std::unique_ptr<MeshPrivate> m_;

};

}
}

#endif // CORE_MESH_H
