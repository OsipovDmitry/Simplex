#include <utils/logger.h>

#include <core/graphicsrendererbase.h>
#include <core/igraphicswidget.h>
#include <core/stateset.h>

#include "graphicsrendererbaseprivate.h"

namespace simplex
{
namespace core
{
namespace graphics
{

core::graphics::PixelInternalFormat pixelNumComponentsAndPixelComponentTypeToPixelInternalFormat(
    uint32_t numComponents,
    utils::PixelComponentType type)
{
    PixelInternalFormat result = core::graphics::PixelInternalFormat::Count;

    switch (type)
    {
        case utils::PixelComponentType::Uint8:
        {
            switch (numComponents)
            {
                case 1:
                {
                    result = core::graphics::PixelInternalFormat::R8;
                    break;
                }
                case 2:
                {
                    result = core::graphics::PixelInternalFormat::RG8;
                    break;
                }
                case 3:
                {
                    result = core::graphics::PixelInternalFormat::RGB8;
                    break;
                }
                case 4:
                {
                    result = core::graphics::PixelInternalFormat::RGBA8;
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        case utils::PixelComponentType::Uint16:
        {
            switch (numComponents)
            {
                case 1:
                {
                    result = core::graphics::PixelInternalFormat::R16;
                    break;
                }
                case 2:
                {
                    result = core::graphics::PixelInternalFormat::RG16;
                    break;
                }
                case 3:
                {
                    result = core::graphics::PixelInternalFormat::RGB16;
                    break;
                }
                case 4:
                {
                    result = core::graphics::PixelInternalFormat::RGBA16;
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        case utils::PixelComponentType::Single:
        {
            switch (numComponents)
            {
                case 1:
                {
                    result = core::graphics::PixelInternalFormat::R16F;
                    break;
                }
                case 2:
                {
                    result = core::graphics::PixelInternalFormat::RG16F;
                    break;
                }
                case 3:
                {
                    result = core::graphics::PixelInternalFormat::RGB16F;
                    break;
                }
                case 4:
                {
                    result = core::graphics::PixelInternalFormat::RGBA16F;
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }

    return result;
}

RendererBase::RendererBase(const std::string& name)
    : m_(std::make_unique<RendererBasePrivate>(name))
{
}

RendererBase::~RendererBase() = default;

const std::string& RendererBase::name() const
{
    return m_->name();
}

bool RendererBase::makeCurrent()
{
    auto currentRenderer = current();

    if (currentRenderer == shared_from_this()) return true;

    currentRenderer->doneCurrent();

    RendererBasePrivate::current() = weak_from_this();
    return doMakeCurrent();
}

bool RendererBase::doneCurrent()
{
    auto currentRenderer = current();

    if (!currentRenderer) return true;

    RendererBasePrivate::current().reset();
    return doDoneCurrent();
}

std::shared_ptr<RendererBase> RendererBase::current()
{
    auto& currentWeak = RendererBasePrivate::current();

    return currentWeak.expired() ? nullptr : currentWeak.lock();
}

bool RendererBase::areShared(
    const std::shared_ptr<const RendererBase>& renderer1,
    const std::shared_ptr<const RendererBase>& renderer2)
{
    if (!renderer1 || !renderer2) return false;

    auto widget1 = renderer1->widget();
    auto widget2 = renderer1->widget();
    if (!widget1 || !widget2) return false;

    return widget1->shareGroup() == widget2->shareGroup();
}

bool RendererBase::registerAttribute(const std::string& name, utils::VertexAttribute ID)
{
    auto& attributeIDs = m_->attributeIDs();

    if (auto it = attributeIDs.find(name); it != attributeIDs.end())
    {
        LOG_ERROR << "Attribute " << name << " is already registered";
        return false;
    }

    attributeIDs.insert({name, ID});
    return true;
}

bool RendererBase::unregisterAttribute(const std::string& name)
{
    auto& attributeIDs = m_->attributeIDs();

    if (auto it = attributeIDs.find(name); it != attributeIDs.end())
    {
        attributeIDs.erase(it);
        return true;
    }

    LOG_ERROR << "Attribute " << name << " has not been registered";
    return false;
}

utils::VertexAttribute RendererBase::attributeByName(const std::string& name) const
{
    auto& attributeIDs = m_->attributeIDs();

    auto it = attributeIDs.find(name);
    return it != attributeIDs.end() ? it->second : utils::VertexAttribute::Count;
}

bool RendererBase::registerOutput(const std::string& name, FrameBufferAttachment ID)
{
    if (!IsFrameBufferColorAttachment(ID))
    {
        LOG_ERROR << "Frame buffer attachment " << name << " must be color";
        return false;
    }

    auto& outputIDs = m_->outputIDs();

    if (auto it = outputIDs.find(name); it != outputIDs.end())
    {
        LOG_ERROR << "Output " << name << " is already registered";
        return false;
    }

    outputIDs.insert({name, ID});
    return true;
}

bool RendererBase::unregisterOutput(const std::string& name)
{
    auto& outputIDs = m_->outputIDs();

    if (auto it = outputIDs.find(name); it != outputIDs.end())
    {
        outputIDs.erase(it);
        return true;
    }

    LOG_ERROR << "Output " << name << " has not been registered";
    return false;
}

FrameBufferAttachment RendererBase::outputByName(const std::string& name) const
{
    auto& outputIDs = m_->outputIDs();

    auto it = outputIDs.find(name);
    return it != outputIDs.end() ? it->second : graphics::FrameBufferAttachment::Count;
}

bool RendererBase::registerUniform(const std::string& name, UniformID ID)
{
    auto& uniformIDs = m_->uniformIDs();

    if (auto it = uniformIDs.find(name); it != uniformIDs.end())
    {
        LOG_ERROR << "Uniform " << name << " is already registered";
        return false;
    }

    uniformIDs.insert({name, ID});
    return true;
}

bool RendererBase::unregisterUniform(const std::string& name)
{
    auto& uniformIDs = m_->uniformIDs();

    if (auto it = uniformIDs.find(name); it != uniformIDs.end())
    {
        uniformIDs.erase(it);
        return true;
    }

    LOG_ERROR << "Uniform " << name << " has not been registered";
    return false;
}

UniformID RendererBase::uniformByName(const std::string& name) const
{
    auto& uniformIDs = m_->uniformIDs();

    auto it = uniformIDs.find(name);
    return it != uniformIDs.end() ? it->second : UniformID::Count;
}

bool RendererBase::registerUniformBlock(const std::string& name, UniformBlockID ID)
{
    auto& uniformBlockIDs = m_->uniformBlockIDs();

    if (auto it = uniformBlockIDs.find(name); it != uniformBlockIDs.end())
    {
        LOG_ERROR << "Uniform block " << name << " is already registered";
        return false;
    }

    uniformBlockIDs.insert({name, ID});
    return true;
}

bool RendererBase::unregisterUniformBlock(const std::string& name)
{
    auto& uniformBlockIDs = m_->uniformBlockIDs();

    if (auto it = uniformBlockIDs.find(name); it != uniformBlockIDs.end())
    {
        uniformBlockIDs.erase(it);
        return true;
    }

    LOG_ERROR << "Uniform block " << name << " has not been registered";
    return false;
}

UniformBlockID RendererBase::uniformBlockByName(const std::string& name) const
{
    auto& uniformBlockIDs = m_->uniformBlockIDs();

    auto it = uniformBlockIDs.find(name);
    return it != uniformBlockIDs.end() ? it->second : core::UniformBlockID::Count;
}

bool RendererBase::registerShaderStorageBlock(const std::string& name, core::ShaderStorageBlockID ID)
{
    auto& shaderStorageBlockIDs = m_->shaderStorageBlockIDs();

    if (auto it = shaderStorageBlockIDs.find(name); it != shaderStorageBlockIDs.end())
    {
        LOG_ERROR << "Shader storage block " << name << " is already registered";
        return false;
    }

    shaderStorageBlockIDs.insert({name, ID});
    return true;
}

bool RendererBase::unregisterShaderStorageBlock(const std::string& name)
{
    auto& shaderStorageBlockIDs = m_->shaderStorageBlockIDs();

    if (auto it = shaderStorageBlockIDs.find(name); it != shaderStorageBlockIDs.end())
    {
        shaderStorageBlockIDs.erase(it);
        return true;
    }

    LOG_ERROR << "Shader storage block " << name << " has not been registered";
    return false;
}

ShaderStorageBlockID RendererBase::shaderStorageBlockByName(const std::string& name) const
{
    auto& shaderStorageBlockIDs = m_->shaderStorageBlockIDs();

    auto it = shaderStorageBlockIDs.find(name);
    return it != shaderStorageBlockIDs.end() ? it->second : core::ShaderStorageBlockID::Count;
}

BufferRange::~BufferRange() = default;

std::shared_ptr<const IBuffer> BufferRange::buffer() const
{
    return m_buffer;
}

size_t BufferRange::offset() const
{
    const auto bufferSize = m_buffer->size();

    if (m_offset > bufferSize)
    {
        LOG_CRITICAL << "Offset is out of range of the buffer size";
        return 0u;
    }

    return m_offset;
}

size_t BufferRange::size() const
{
    const auto bufferSize = m_buffer->size();
    auto result = m_size;

    if (result == static_cast<size_t>(-1))
    {
        result = bufferSize - m_offset;
    }

    if (m_offset + result > bufferSize)
    {
        LOG_CRITICAL << "Offset and size are out of range of the buffer size";
        return 0u;
    }

    return result;
}

std::shared_ptr<BufferRange> BufferRange::create(const std::shared_ptr<const IBuffer>& buffer, size_t offset, size_t size)
{
    if (!buffer)
    {
        LOG_CRITICAL << "Buffer can't be nullptr";
        return nullptr;
    }

    return std::shared_ptr<BufferRange>(new BufferRange(buffer, offset, size));
}

BufferRange::BufferRange(const std::shared_ptr<const IBuffer>& buffer, size_t offset, size_t size)
    : m_buffer(buffer)
    , m_offset(offset)
    , m_size(size)
{
}

Image::~Image() = default;

Image::DataAccess Image::access() const
{
    return m_access;
}

PConstTexture Image::texture() const
{
    return m_texture;
}

uint32_t Image::mipmapLevel() const
{
    return m_mipmapLevel;
}

const SupportedImageFormats& Image::supportedImageFormats()
{
    static const SupportedImageFormats s_supportdFormats{
        {PixelInternalFormat::RGBA32F, "rgba32f"},
        {PixelInternalFormat::RGBA16F, "rgba16f"},
        {PixelInternalFormat::RG32F, "rg32f"},
        {PixelInternalFormat::RG16F, "rg16f"},
        {PixelInternalFormat::R11F_G11F_B10F, "r11f_g11f_b10f"},
        {PixelInternalFormat::R32F, "r32f"},
        {PixelInternalFormat::R16F, "r16f"},
        {PixelInternalFormat::RGBA32UI, "rgba32ui"},
        {PixelInternalFormat::RGBA16UI, "rgba16ui"},
        {PixelInternalFormat::RGB10_A2UI, "rgb10a2ui"},
        {PixelInternalFormat::RGBA8UI, "rgba8ui"},
        {PixelInternalFormat::RG32UI, "rg32ui"},
        {PixelInternalFormat::RG16UI, "rg16ui"},
        {PixelInternalFormat::RG8UI, "rg8ui"},
        {PixelInternalFormat::R32UI, "r32ui"},
        {PixelInternalFormat::R16UI, "r18ui"},
        {PixelInternalFormat::R8UI, "r8ui"},
        {PixelInternalFormat::RGBA32I, "rgba32i"},
        {PixelInternalFormat::RGBA16I, "rgba16i"},
        {PixelInternalFormat::RGBA8I, "rgba8i"},
        {PixelInternalFormat::RG32I, "rg32i"},
        {PixelInternalFormat::RG16I, "rg16i"},
        {PixelInternalFormat::RG8I, "rg8i"},
        {PixelInternalFormat::R32I, "rg32i"},
        {PixelInternalFormat::R16I, "rg16i"},
        {PixelInternalFormat::R8I, "r8i"},
        {PixelInternalFormat::RGBA16, "rgba16"},
        {PixelInternalFormat::RGB10_A2, "rgb10a2"},
        {PixelInternalFormat::RGBA8, "rgba8"},
        {PixelInternalFormat::RG16, "rg16"},
        {PixelInternalFormat::RG8, "rg8"},
        {PixelInternalFormat::R16, "r16"},
        {PixelInternalFormat::R8, "r8"},
        {PixelInternalFormat::RGBA16_SNORM, "rgba16_snorm"},
        {PixelInternalFormat::RGBA8_SNORM, "rgba8_snorm"},
        {PixelInternalFormat::RG16_SNORM, "rg16_snorm"},
        {PixelInternalFormat::RG8_SNORM, "rg8_snorm"},
        {PixelInternalFormat::R16_SNORM, "r16_snorm"},
        {PixelInternalFormat::R8_SNORM, "r8_snorm"}};

    return s_supportdFormats;
}

std::shared_ptr<Image> Image::create(DataAccess access, const PConstTexture& texture, uint32_t mipmapLevel)
{
    if (access == DataAccess::Count)
    {
        LOG_CRITICAL << "Undefined image data access";
        return nullptr;
    }

    if (!texture)
    {
        LOG_CRITICAL << "Texture can't be nullptr";
        return nullptr;
    }

    if (mipmapLevel >= texture->numMipmapLevels())
    {
        LOG_CRITICAL << "Level must be less than texture levels count";
        return nullptr;
    }

    if (const auto internalFormat = texture->internalFormat();
        supportedImageFormats().find(internalFormat) == supportedImageFormats().end())
        LOG_CRITICAL << "Unsupported image format";

    return std::shared_ptr<Image>(new Image(access, texture, mipmapLevel));
}

Image::Image(DataAccess access, const PConstTexture& texture, uint32_t mipmapLevel)
    : m_access(access)
    , m_texture(texture)
    , m_mipmapLevel(mipmapLevel)
{
}

} // namespace graphics
} // namespace core
} // namespace simplex