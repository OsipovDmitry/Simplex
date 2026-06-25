#ifndef CORE_LIGHTNODEPRIVATE_H
#define CORE_LIGHTNODEPRIVATE_H

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

    LightType& type();
    bool& isLightingEnabled();

    void onChanged();

protected:
    virtual std::shared_ptr<LightHandler> createLightInSceneData(SceneData&) const = 0;
    virtual void updateLightInSceneData(LightHandler&) const = 0;

    LightType m_type;
    bool m_isLightingEnabled;

private:
    std::shared_ptr<LightHandler> m_lightHandler;
};

} // namespace core
} // namespace simplex

#endif // CORE_LIGHTNODEPRIVATE_H
