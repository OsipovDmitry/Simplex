#ifndef CORE_DRAWABLE_H
#define CORE_DRAWABLE_H

#include <utils/noncopyble.h>

#include <utils/noncopyble.h>
#include <utils/forwarddecl.h>

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

public:
    explicit Drawable(const std::shared_ptr<graphics::IVertexArray>&);
    virtual ~Drawable();

    virtual bool isTransparent() const;

    utils::BoundingBox calculateBoundingBox() const;

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
    static graphics::PBRComponentsSet PBRComponentsSet(const std::shared_ptr<const Drawable>&);
    static graphics::LightComponentsSet lightComponentsSet(const std::shared_ptr<const Drawable>&);

protected:
    Drawable(std::unique_ptr<DrawablePrivate>);

    std::unique_ptr<DrawablePrivate> m_;
};

}
}

#endif // CORE_DRAWABLE_H
