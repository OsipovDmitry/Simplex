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

    std::shared_ptr<Drawable> &areaDrawable();

    bool &isAreaMatrixDirty();
    glm::mat4x4 &areaMatrix();

private:
    bool m_isLightingEnabled;
    std::shared_ptr<Drawable> m_areaDrawable;
    bool m_isAreaMatrixDirty;
    glm::mat4x4 m_areaMatrix;

};

}
}

#endif // CORE_LIGHTNODEPRIVATE_H
