#ifndef CORE_CAMERANODEPRIVATE_H
#define CORE_CAMERANODEPRIVATE_H

#include <utils/clipspace.h>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class GeometryBuffer;

class CameraNodePrivate : public NodePrivate
{
public:
    CameraNodePrivate(CameraNode&, const std::string&);
    ~CameraNodePrivate() override;

    bool &isRenderingEnabled();

    std::shared_ptr<GeometryBuffer> &geometryBuffer();

    utils::ClipSpaceType &clipSpaceType();
    float &clipSpaceVerticalParam();
    utils::ClipSpace &clipSpace();

    utils::Range &cullPlanesLimits();
    glm::uvec3& clusterMaxSize();

    void resize(const glm::uvec2&);
    void updateClipSpace();

private:
    bool m_isRenderingEnabled;

    std::shared_ptr<GeometryBuffer> m_geometryBuffer;

    utils::ClipSpaceType m_clipSpaceType;
    float m_clipSpaceVerticalParam;
    float m_viewportAspectRatio;
    utils::ClipSpace m_clipSpace;

    utils::Range m_cullPlanesLimits;
    glm::uvec3 m_clusterMaxSize;
};

}
}

#endif // CORE_CAMERANODEPRIVATE_H
