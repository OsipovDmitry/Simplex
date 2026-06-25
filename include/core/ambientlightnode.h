#ifndef CORE_AMBIENTLIGHTNODE_H
#define CORE_AMBIENTLIGHTNODE_H

#include <utils/glm/vec3.hpp>

#include <core/coloredlightnode.h>

namespace simplex
{
namespace core
{

class AmbientLightNodePrivate;
class CORE_SHARED_EXPORT AmbientLightNode : public ColoredLightNode
{
    PRIVATE_IMPL(AmbientLightNode)
public:
    AmbientLightNode(const std::string&);
    ~AmbientLightNode() override;

    std::shared_ptr<AmbientLightNode> asAmbientLightNode() override;
    std::shared_ptr<const AmbientLightNode> asAmbientLightNode() const override;
};

} // namespace core
} // namespace simplex

#endif // CORE_AMBIENTLIGHTNODE_H
