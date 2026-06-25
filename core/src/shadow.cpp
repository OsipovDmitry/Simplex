#include "shadow.h"

#include <core/shadowedlightnode.h>

#include "scenedata.h"
#include "shadowedlightnodeprivate.h"

namespace simplex
{
namespace core
{

Shadow::Shadow(const ShadowedLightNode& shadowedLightNode)
    : m_shadowedLightNode(shadowedLightNode)
{
}

Shadow::~Shadow() = default;

const ShadowedLightNode& Shadow::shadowedLightNode() const
{
    return m_shadowedLightNode;
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
                *m_handler, m_shadowedLightNode.shadowMapSize(), m_shadowedLightNode.shadowCullPlanesLimits(),
                m_shadowedLightNode.m().shadowLayersCount());
        }
    }
}

} // namespace core
} // namespace simplex
