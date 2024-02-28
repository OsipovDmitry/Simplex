#ifndef CORE_DRAWABLE_H
#define CORE_DRAWABLE_H

#include <utils/noncopyble.h>

#include <utils/noncopyble.h>
#include <utils/forwarddecl.h>
#include <utils/enumclass.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

ENUMCLASS(DrawableAlphaMode, uint16_t,
          Opaque,
          Mask,
          Transparent)

class DrawablePrivate;
class CORE_SHARED_EXPORT Drawable
{
    NONCOPYBLE(Drawable)

public:
    Drawable(const std::shared_ptr<graphics::IVertexArray>&);
    virtual ~Drawable();

    virtual const utils::BoundingBox &boundingBox() const;

    bool isDoubleSided() const;
    void setDoubleSided(bool);

    float alphaCutoff() const;
    void setAlphaCutoff(float);

    virtual DrawableAlphaMode alphaMode() const;

    std::shared_ptr<const graphics::IVertexArray> vertexArray() const;
    std::shared_ptr<graphics::IVertexArray> vertexArray();

    PConstAbstractUniform uniform(graphics::UniformId) const;
    PAbstractUniform uniform(graphics::UniformId);
    PAbstractUniform &getOrCreateUniform(graphics::UniformId);

    PConstAbstractUniform userUniform(const std::string&) const;
    PAbstractUniform userUniform(const std::string&);
    PAbstractUniform &getOrCreateUserUniform(const std::string&);

    graphics::PConstBufferRange SSBO(graphics::SSBOId) const;

    static utils::VertexAttributesSet vertexAttrubitesSet(const std::shared_ptr<const Drawable>&);

protected:
    Drawable(std::unique_ptr<DrawablePrivate>);

    std::unique_ptr<DrawablePrivate> m_;
};

}
}

#endif // CORE_DRAWABLE_H
