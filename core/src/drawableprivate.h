#ifndef CORE_DRAWABLEPRIVATE_H
#define CORE_DRAWABLEPRIVATE_H

#include <memory>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class DrawableHandler;

class DrawablePrivate
{
public:
    DrawablePrivate(const std::shared_ptr<const Mesh>&, const std::shared_ptr<const Material>& material);
    ~DrawablePrivate();

    std::shared_ptr<const Mesh> &mesh();
    std::shared_ptr<const Material> &material();

    std::set<std::shared_ptr<DrawableHandler>>& handlers();
    void onChanged();

protected:
    std::shared_ptr<const Mesh> m_mesh;
    std::shared_ptr<const Material> m_material;
    std::set<std::shared_ptr<DrawableHandler>> m_handlers;
};

}
}

#endif // CORE_DRAWABLEPRIVATE_H
