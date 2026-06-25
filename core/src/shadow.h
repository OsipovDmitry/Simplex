#ifndef CORE_SHADOW_H
#define CORE_SHADOW_H

#include <memory>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class ShadowHandler;

class Shadow
{
public:
    Shadow(const ShadowedLightNode&);
    ~Shadow();

    const ShadowedLightNode& shadowedLightNode() const;
    std::shared_ptr<ShadowHandler>& handler() const;

    void onChanged();

private:
    const ShadowedLightNode& m_shadowedLightNode;
    mutable std::shared_ptr<ShadowHandler> m_handler;
};

} // namespace core
} // namespace simplex

#endif // CORE_SHADOW_H
