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
    Shadow(const LightNode&);
    ~Shadow();

    const LightNode& lightNode() const;
    std::shared_ptr<ShadowHandler>& handler() const;

    void onChanged();

private:
    const LightNode& m_lightNode;
    mutable std::shared_ptr<ShadowHandler> m_handler;
};

} // namespace core
} // namespace simplex

#endif // CORE_SHADOW_H
