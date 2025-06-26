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
    ~Drawable() override;

    std::shared_ptr<const graphics::IVertexArray> vertexArray() const;
    std::shared_ptr<graphics::IVertexArray> vertexArray();
    utils::VertexAttributeSet vertexAttrubiteSet() const;

    bool isDoubleSided() const;
    void setDoubleSided(bool);

protected:
    Drawable(std::unique_ptr<DrawablePrivate>);

};

}
}

#endif // CORE_DRAWABLE_H
