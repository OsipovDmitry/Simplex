#ifndef CORE_LIGHTNODEPRIVATE_H
#define CORE_LIGHTNODEPRIVATE_H

#include <utils/clipspace.h>
#include <utils/frustum.h>
#include <utils/idgenerator.h>

#include <core/lightnode.h>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class LightHandler;
class SceneData;

class Shadow;

class LightNodePrivate : public NodePrivate
{
public:
    LightNodePrivate(LightNode&, const std::string&, LightType);
    ~LightNodePrivate() override;

    void onAfterTransformChanged() override;
    void onAttachToScene(const std::shared_ptr<Scene>&) override;
    void onDetachFromScene(const std::shared_ptr<Scene>&) override;

    virtual uint32_t shadowLayersCount() const = 0;

    LightType& type();
    bool& isLightingEnabled();

    std::shared_ptr<Shadow>& shadow();
    uint32_t& shadowMapSize();
    utils::Range& shadowCullPlanesLimits();

    void onChanged();

protected:
    virtual std::shared_ptr<LightHandler> createLightInSceneData(SceneData&) const = 0;
    virtual void updateLightInSceneData(SceneData&, utils::IDsGeneratorT<size_t>::value_type) const = 0;

    LightType m_type;
    bool m_isLightingEnabled;

    std::shared_ptr<Shadow> m_shadow;
    uint32_t m_shadowMapSize;
    utils::Range m_shadowCullPlanesLimits;

private:
    std::shared_ptr<LightHandler> m_lightHandler;
};

} // namespace core
} // namespace simplex

#endif // CORE_LIGHTNODEPRIVATE_H
