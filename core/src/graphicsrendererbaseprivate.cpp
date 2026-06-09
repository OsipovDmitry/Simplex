#include "graphicsrendererbaseprivate.h"

namespace simplex
{
namespace core
{
namespace graphics
{

std::weak_ptr<RendererBase> RendererBasePrivate::s_current;

RendererBasePrivate::RendererBasePrivate(const std::string& name)
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

std::unordered_map<std::string, core::UniformID>& RendererBasePrivate::uniformIDs()
{
    return m_uniformIDs;
}

std::unordered_map<std::string, core::UniformBlockID>& RendererBasePrivate::uniformBlockIDs()
{
    return m_uniformBlockIDs;
}

std::unordered_map<std::string, core::ShaderStorageBlockID>& RendererBasePrivate::shaderStorageBlockIDs()
{
    return m_shaderStorageBlockIDs;
}

std::weak_ptr<RendererBase>& RendererBasePrivate::current()
{
    return s_current;
}

} // namespace graphics
} // namespace core
} // namespace simplex