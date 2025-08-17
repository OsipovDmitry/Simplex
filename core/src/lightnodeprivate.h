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
class LightDrawable;

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

    utils::BoundingBox doBoundingBox() override;

    LightType& type();

    static uint32_t numLayeredShadowMatrices(LightType type);
    uint32_t numLayeredShadowMatrices() const;

    bool &isLightingEnabled();

    Shadow &shadow();

    ShadowTransform &shadowTransform(const utils::Frustum::Points&);
    std::shared_ptr<LightDrawable> &areaDrawable();
    const glm::mat4x4 &areaMatrix();
    const utils::BoundingBox &areaBoundingBox();

    void dirtyAreaMatrix();
    void dirtyAreaBoundingBox();

private:
    LightType m_type;

    bool m_isLightingEnabled;

    std::shared_ptr<Shadow> m_shadow;

    ShadowTransform m_shadowTransform;
    std::shared_ptr<LightDrawable> m_areaDrawable;
    glm::mat4x4 m_areaMatrix;
    utils::BoundingBox m_areaBoundingBox;

    bool m_isAreaMatrixDirty;
    bool m_isAreaBoundingBoxDirty;

    virtual ShadowTransform doShadowTransform(const utils::Frustum::Points&);

    virtual glm::mat4x4 doAreaMatrix() = 0;
    virtual utils::BoundingBox doAreaBoundingBox() = 0;
};

}
}

#endif // CORE_LIGHTNODEPRIVATE_H
