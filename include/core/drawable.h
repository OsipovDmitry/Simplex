#ifndef CORE_DRAWABLE_H
#define CORE_DRAWABLE_H

#include <utils/noncopyble.h>
#include <utils/pimpl.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class DrawablePrivate;
class CORE_SHARED_EXPORT Drawable
{
    NONCOPYBLE(Drawable)
    PRIVATE_IMPL(Drawable)

public:
    Drawable(const std::shared_ptr<Mesh>&, const std::shared_ptr<Material>&);
    ~Drawable();

    std::shared_ptr<const Mesh> mesh() const;
    void setMesh(const std::shared_ptr<const Mesh>&);

    std::shared_ptr<const Material> material() const;
    void setMaterial(const std::shared_ptr<const Material>&);

private:
    std::unique_ptr<DrawablePrivate> m_;

};

}
}

#endif // CORE_DRAWABLE_H
