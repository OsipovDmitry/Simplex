#ifndef CORE_AMBIENTLIGHTNODE_H
#define CORE_AMBIENTLIGHTNODE_H

#include <utils/glm/vec3.hpp>

#include <core/lightnode.h>

namespace simplex
{
namespace core
{

class AmbientLightNodePrivate;
class CORE_SHARED_EXPORT AmbientLightNode : public LightNode
{
    PRIVATE_IMPL(AmbientLightNode)
public:
    AmbientLightNode(const std::string&);
    ~AmbientLightNode() override;

    std::shared_ptr<AmbientLightNode> asAmbientLightNode() override;
    std::shared_ptr<const AmbientLightNode> asAmbientLightNode() const override;

    const glm::vec3& color() const;
    void setColor(const glm::vec3&);
};

} // namespace core
} // namespace simplex

#endif // CORE_AMBIENTLIGHTNODE_H
