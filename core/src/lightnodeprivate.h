#ifndef CORE_LIGHTNODEPRIVATE_H
#define CORE_LIGHTNODEPRIVATE_H

#include <core/forwarddecl.h>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class LightNodePrivate : public NodePrivate
{
public:
    LightNodePrivate(const std::string &name);
    ~LightNodePrivate() override;

    bool &isLightingEnabled();
    bool &isShadingEnabled();

    std::shared_ptr<LightDrawable> &areaDrawable();

    bool &isAreaMatrixDirty();
    glm::mat4x4 &areaMatrix();

    bool &isAreaBoundingBoxDirty();
    utils::BoundingBox &areaBoundingBox();

private:
    bool m_isLightingEnabled;
    bool m_isShadingEnabled;

    std::shared_ptr<LightDrawable> m_areaDrawable;

    bool m_isAreaMatrixDirty;
    glm::mat4x4 m_areaMatrix;

    bool m_isAreaBoundingBoxDirty;
    utils::BoundingBox m_areaBoundingBox;
};

}
}

#endif // CORE_LIGHTNODEPRIVATE_H
