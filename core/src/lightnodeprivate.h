#ifndef CORE_LIGHTNODEPRIVATE_H
#define CORE_LIGHTNODEPRIVATE_H

#include <utils/frustum.h>
#include <utils/clipspace.h>

#include <core/lightnode.h>
#include <core/shadow.h>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class ShadowPrivate;

class LightNodePrivate : public NodePrivate
{
public:
    struct ShadowTransform
    {
        utils::Transform frustumViewTransform;
        utils::ClipSpace frustumClipSpace;

        std::vector<utils::Transform> layeredViewTransforms;
        utils::ClipSpace clipSpase;
        utils::Range cullPlanesLimits;
    };

    LightNodePrivate(LightNode&, const std::string&, LightType);
    ~LightNodePrivate() override;

    LightType& type();

    static uint32_t numLayeredShadowMatrices(LightType type);
    uint32_t numLayeredShadowMatrices() const;

    bool &isLightingEnabled();

    Shadow &shadow();

    ShadowTransform &shadowTransform(const utils::Frustum::Points&);

private:
    LightType m_type;

    bool m_isLightingEnabled;

    std::shared_ptr<Shadow> m_shadow;
    ShadowTransform m_shadowTransform;

    virtual ShadowTransform doShadowTransform(const utils::Frustum::Points&);
};

}
}

#endif // CORE_LIGHTNODEPRIVATE_H
