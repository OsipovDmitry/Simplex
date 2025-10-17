#ifndef CORE_CAMERANODEPRIVATE_H
#define CORE_CAMERANODEPRIVATE_H

#include <utils/clipspace.h>

#include <core/ssao.h>

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

    SSAO &ssao();

    void resize(const glm::uvec2&);

private:
    bool m_isRenderingEnabled;

    std::shared_ptr<GeometryBuffer> m_geometryBuffer;

    utils::ClipSpaceType m_clipSpaceType;
    float m_clipSpaceVerticalParam;
    utils::ClipSpace m_clipSpace;

    utils::Range m_cullPlanesLimits;

    SSAO m_ssao;
};

}
}

#endif // CORE_CAMERANODEPRIVATE_H
