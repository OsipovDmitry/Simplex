#ifndef CORE_LIGHTNODE_H
#define CORE_LIGHTNODE_H

#include <utils/glm/vec3.hpp>
#include <utils/glm/mat4x4.hpp>
#include <utils/pimpl.h>
#include <utils/enumclass.h>

#include <core/forwarddecl.h>
#include <core/node.h>

namespace simplex
{
namespace core
{

class LightNodePrivate;
class CORE_SHARED_EXPORT LightNode : public Node
{
    PIMPL(LightNode)
public:
    ~LightNode() override;

    std::shared_ptr<LightNode> asLightNode() override;
    std::shared_ptr<const LightNode> asLightNode() const override;

    bool isLightingEnabled() const;
    void setLightingEnabled(bool);

    const glm::mat4x4 &areaMatrix() const;
    std::shared_ptr<const Drawable> areaDrawable() const;

protected:
    LightNode(std::unique_ptr<LightNodePrivate>);

    bool canAttach(std::shared_ptr<Node>) override;
    bool canDetach(std::shared_ptr<Node>) override;

    virtual glm::mat4x4 doAreaMatrix() const = 0;
};

}
}

#endif // CORE_LIGHTNODE_H
