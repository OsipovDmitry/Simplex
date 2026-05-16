#include "shadow.h"

#include <core/lightnode.h>

#include "lightnodeprivate.h"
#include "scenedata.h"

namespace simplex
{
namespace core
{

Shadow::Shadow(const LightNode& lightNode)
    : m_lightNode(lightNode)
{
}

Shadow::~Shadow() = default;

const LightNode& Shadow::lightNode() const
{
    return m_lightNode;
}

std::shared_ptr<ShadowHandler>& Shadow::handler() const
{
    return m_handler;
}

void Shadow::onChanged()
{
    if (m_handler)
    {
        if (auto sceneData = m_handler->sceneData().lock())
        {
            sceneData->onShadowChanged(
                m_handler->ID(), m_lightNode.shadowMapSize(), m_lightNode.shadowCullPlanesLimits(),
                m_lightNode.m().shadowLayersCount());
        }
    }
}

} // namespace core
} // namespace simplex
