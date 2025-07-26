#ifndef CORE_DRAWABLE_H
#define CORE_DRAWABLE_H

#include <utils/noncopyble.h>
#include <utils/pimpl.h>
#include <utils/forwarddecl.h>
#include <utils/enumclass.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/stateset.h>

namespace simplex
{
namespace core
{

class DrawablePrivate;
class CORE_SHARED_EXPORT Drawable : public StateSet
{
    PRIVATE_IMPL(Drawable)

public:
    Drawable(const std::shared_ptr<graphics::IVertexArray>&);
    Drawable(const std::shared_ptr<Mesh>&, const std::shared_ptr<Material>&);
    ~Drawable() override;

    std::shared_ptr<const Mesh> mesh() const;
    void setMesh(const std::shared_ptr<const Mesh>&);

    std::shared_ptr<const Material> material() const;
    void setMaterial(const std::shared_ptr<const Material>&);

    // tmp
    std::shared_ptr<const graphics::IVertexArray> vertexArray() const;
    std::shared_ptr<graphics::IVertexArray> vertexArray();
    void setVertexArray(std::shared_ptr<graphics::IVertexArray>);
    utils::VertexAttributeSet vertexAttrubiteSet() const;

    bool isDoubleSided() const;
    void setDoubleSided(bool);

protected:
    Drawable(std::unique_ptr<DrawablePrivate>);

};

}
}

#endif // CORE_DRAWABLE_H
