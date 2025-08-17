#include <utils/glm/common.hpp>

#include <core/graphicsrendererbase.h>
#include <core/shadow.h>

#include "shadowbuffer.h"

namespace simplex
{
namespace core
{

ShadowBuffer::ShadowBuffer()
    : m_size(0u)
    , m_shadingMode(ShadingMode::Count)
    , m_shadingFilter(ShadingFilter::Count)
{
}

ShadowBuffer::~ShadowBuffer() = default;

const glm::uvec2& ShadowBuffer::size() const
{
    return m_size;
}

ShadingMode ShadowBuffer::shadingMode() const
{
    return m_shadingMode;
}

ShadingFilter ShadowBuffer::shadingFilter() const
{
    return m_shadingFilter;
}

void ShadowBuffer::update(const glm::uvec2& size, ShadingMode shadingMode, ShadingFilter shadingFIlter)
{
    bool needToRecreate = false;
    
    if (m_size != size)
        needToRecreate = true;
    if (m_shadingMode != shadingMode)
        needToRecreate = true;
    if (m_shadingFilter != shadingFIlter)
        needToRecreate = true;

    if (!needToRecreate)
        return;

    m_size = glm::max(size, glm::uvec2(1u, 1u));
    m_shadingMode = shadingMode;
    m_shadingFilter = shadingFIlter;

    if (m_shadingMode != ShadingMode::Disabled)
    {
        m_depthTexture = doTexture(graphics::PixelInternalFormat::Depth32F, graphics::TextureFilterMode::Point);

        if (m_shadingMode == ShadingMode::Color)
            m_colorTexture = doTexture(graphics::PixelInternalFormat::R11F_G11F_B10F, graphics::TextureFilterMode::Linear);

        if (m_shadingFilter == ShadingFilter::VSM)
            m_depthVSMTexture = doTexture(graphics::PixelInternalFormat::RG32F/*RG16F*/, graphics::TextureFilterMode::Linear);
    }
}

graphics::PConstTexture ShadowBuffer::colorTexture() const
{
    return m_colorTexture;
}

graphics::PConstTexture ShadowBuffer::depthTexture() const
{
    return m_depthTexture;
}

graphics::PConstTexture ShadowBuffer::depthVSMTexture() const
{
    return m_depthVSMTexture;
}

graphics::PTexture ShadowBuffer::doTexture(graphics::PixelInternalFormat, graphics::TextureFilterMode) const
{
    return nullptr;
}

ShadowBuffer2D::ShadowBuffer2D()
    : ShadowBuffer()
{
}

ShadowBuffer2D::~ShadowBuffer2D() = default;

graphics::PTexture ShadowBuffer2D::doTexture(
    graphics::PixelInternalFormat pixelInternalFormat,
    graphics::TextureFilterMode textureFilterMode) const
{
    auto graphicsRenderer = graphics::RendererBase::current();
    if (!graphicsRenderer)
    {
        LOG_CRITICAL << "No current graphics renderer";
        return nullptr;
    }

    const auto& textureSize = size();

    auto result = graphicsRenderer->createTexture2DEmpty(textureSize.x, textureSize.y, pixelInternalFormat);
    result->setFilterMode(textureFilterMode);
    result->setWrapMode(graphics::TextureWrapMode::ClampToBorder);
    result->setBorderColor(glm::vec4(1.f));

    return result;
}

ShadowBufferCube::ShadowBufferCube()
    : ShadowBuffer()
{
}

ShadowBufferCube::~ShadowBufferCube() = default;

graphics::PTexture ShadowBufferCube::doTexture(
    graphics::PixelInternalFormat pixelInternalFormat,
    graphics::TextureFilterMode textureFilterMode) const
{
    auto graphicsRenderer = graphics::RendererBase::current();
    if (!graphicsRenderer)
    {
        LOG_CRITICAL << "No current graphics renderer";
        return nullptr;
    }

    const auto& textureSize = size();

    auto result = graphicsRenderer->createTextureCubeEmpty(textureSize.x, textureSize.y, pixelInternalFormat);
    result->setFilterMode(textureFilterMode);
    result->setWrapMode(graphics::TextureWrapMode::ClampToBorder);
    result->setBorderColor(glm::vec4(1.f));

    return result;
}

}
}
