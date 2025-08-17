#include "graphicsrendererbaseprivate.h"

namespace simplex
{
namespace core
{
namespace graphics
{

std::weak_ptr<RendererBase> RendererBasePrivate::s_current;

RendererBasePrivate::RendererBasePrivate(const std::string &name)
    : m_name(name)
{
}

const std::string& RendererBasePrivate::name()
{
    return m_name;
}

std::unordered_map<std::string, utils::VertexAttribute>& RendererBasePrivate::attributeIDs()
{
    return m_attributeIDs;
}

std::unordered_map<std::string, core::graphics::FrameBufferAttachment>& RendererBasePrivate::outputIDs()
{
    return m_outputIDs;
}

std::unordered_map<std::string, core::UniformId>& RendererBasePrivate::uniformIDs()
{
    return m_uniformIDs;
}

std::unordered_map<std::string, core::SSBOId>& RendererBasePrivate::SSBOIDs()
{
    return m_SSBOIDs;
}

std::weak_ptr<RendererBase>& RendererBasePrivate::current()
{
    return s_current;
}

DynamicBufferBasePrivate::DynamicBufferBasePrivate(const std::shared_ptr<IBuffer>& buffer)
    : m_buffer(buffer)
    , m_size(0u)
{
}

std::shared_ptr<IBuffer>& DynamicBufferBasePrivate::buffer()
{
    return m_buffer;
}

size_t& DynamicBufferBasePrivate::size()
{
    return m_size;
}

}
}
}