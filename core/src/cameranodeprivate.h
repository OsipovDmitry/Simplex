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
class GFrameBufferTmp;
class SSAOFrameBuffer;
class BlurFrameBuffer;
class PostprocessFrameBuffer;

class CameraNodePrivate : public NodePrivate
{
public:
    CameraNodePrivate(CameraNode&, const std::string&, /*tmp*/const std::shared_ptr<graphics::IFrameBuffer>&);
    ~CameraNodePrivate() override;

    bool &isRenderingEnabled();

    std::shared_ptr<GeometryBuffer> &geometryBuffer();

    utils::ClipSpaceType &clipSpaceType();
    float &clipSpaceVerticalParam();
    utils::ClipSpace &clipSpace();

    utils::Range &cullPlanesLimits();

    SSAO &ssao();

    std::shared_ptr<GFrameBufferTmp> &gFrameBuffer();
    std::shared_ptr<SSAOFrameBuffer> &ssaoFrameBuffer();
    std::shared_ptr<BlurFrameBuffer> &ssaoBlurFrameBuffer();
    std::shared_ptr<PostprocessFrameBuffer> &postprocessFrameBuffer();

    void resize(const std::shared_ptr<graphics::RendererBase>&, const std::shared_ptr<graphics::IFrameBuffer>&, const std::shared_ptr<GeometryBuffer>&);

private:
    bool m_isRenderingEnabled;

    std::shared_ptr<GeometryBuffer> m_geometryBuffer;

    /*tmp*/std::shared_ptr<graphics::IFrameBuffer> m_defaultFrameBuffer;

    utils::ClipSpaceType m_clipSpaceType;
    float m_clipSpaceVerticalParam;
    utils::ClipSpace m_clipSpace;

    utils::Range m_cullPlanesLimits;

    SSAO m_ssao;

    std::shared_ptr<GFrameBufferTmp> m_gFrameBuffer;
    std::shared_ptr<SSAOFrameBuffer> m_ssaoFrameBuffer;
    std::shared_ptr<BlurFrameBuffer> m_ssaoBlurFrameBuffer;
    std::shared_ptr<PostprocessFrameBuffer> m_postprocessFrameBuffer;
};

}
}

#endif // CORE_CAMERANODEPRIVATE_H
