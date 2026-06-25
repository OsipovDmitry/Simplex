#ifndef CORE_AMBIENTLIGHTNODEPRIVATE_H
#define CORE_AMBIENTLIGHTNODEPRIVATE_H

#include "coloredlightnodeprivate.h"

namespace simplex
{
namespace core
{

class AmbientLightNodePrivate : public ColoredLightNodePrivate
{
    PUBLIC_IMPL(AmbientLightNode)
public:
    AmbientLightNodePrivate(AmbientLightNode&, const std::string& name);
    ~AmbientLightNodePrivate() override;

private:
    std::shared_ptr<LightHandler> createLightInSceneData(SceneData&) const override;
    void updateLightInSceneData(LightHandler&) const override;
};

} // namespace core
} // namespace simplex

#endif // CORE_AMBIENTLIGHTNODEPRIVATE_H
