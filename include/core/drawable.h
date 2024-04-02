#ifndef CORE_DRAWABLE_H
#define CORE_DRAWABLE_H

#include <utils/noncopyble.h>

#include <utils/noncopyble.h>
#include <utils/forwarddecl.h>
#include <utils/enumclass.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/igraphicsrenderer.h>

namespace simplex
{
namespace core
{

ENUMCLASS(DrawableAlphaMode, uint16_t,
          Opaque,
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
    void removeUniform(graphics::UniformId);

    PConstAbstractUniform userUniform(const std::string&) const;
    PAbstractUniform userUniform(const std::string&);
    PAbstractUniform &getOrCreateUserUniform(const std::string&);
    void removeUserUniform(const std::string&);

    graphics::PConstBufferRange SSBO(graphics::SSBOId) const;
    graphics::PConstBufferRange &getOrCreateSSBO(graphics::SSBOId);
    void removeSSBO(graphics::SSBOId);

    utils::VertexAttributesSet vertexAttrubitesSet() const;

    template<typename T, typename UniformIdConvertMethod>
    std::unordered_set<T> componentsSet(UniformIdConvertMethod method) const;

protected:
    Drawable(std::unique_ptr<DrawablePrivate>);

    std::unique_ptr<DrawablePrivate> m_;
};

template<typename T, typename UniformIdByComponentMethod>
std::unordered_set<T> Drawable::componentsSet(UniformIdByComponentMethod method) const
{
    std::unordered_set<T> result;

    for (typename std::underlying_type<T>::type i = 0u; i < utils::numElementsEnumClass<T>(); ++i)
    {
        auto component = utils::castToEnumClass<T>(i);
        if (graphics::UniformId uniformId = method(component);
                (uniformId != graphics::UniformId::Undefined) && uniform(uniformId))
            result.insert(component);
    }

    return result;
}

}
}

#endif // CORE_DRAWABLE_H
