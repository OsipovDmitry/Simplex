#ifndef CORE_MESHPRIVATE_H
#define CORE_MESHPRIVATE_H

#include <utils/boundingbox.h>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class MeshHandler;

class MeshPrivate
{
public:
    MeshPrivate();
    ~MeshPrivate();

    std::shared_ptr<utils::Mesh>& mesh();
    utils::BoundingBox& boundingBox();

    std::set<std::shared_ptr<MeshHandler>>& handlers();
    void onChanged();

private:
    std::shared_ptr<utils::Mesh> m_mesh;
    utils::BoundingBox m_boundingBox;
    std::set<std::shared_ptr<MeshHandler>> m_handlers;
};

}
}

#endif // CORE_MESHPRIVATE_H
