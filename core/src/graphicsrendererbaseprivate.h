#ifndef CORE_GRAPHICSRENDERERBASEPRIVATE_H
#define CORE_GRAPHICSRENDERERBASEPRIVATE_H

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{
namespace graphics
{

class RendererBasePrivate
{
public:
    RendererBasePrivate(const std::string& name);

    const const std::string& name();

    std::unordered_map<std::string, utils::VertexAttribute>& attributeIDs();
    std::unordered_map<std::string, core::graphics::FrameBufferAttachment>& outputIDs();
    std::unordered_map<std::string, core::UniformID>& uniformIDs();
    std::unordered_map<std::string, core::UniformBlockID>& uniformBlockIDs();
    std::unordered_map<std::string, core::ShaderStorageBlockID>& shaderStorageBlockIDs();

    static std::weak_ptr<RendererBase>& current();

private:
    std::string m_name;

    std::unordered_map<std::string, utils::VertexAttribute> m_attributeIDs;
    std::unordered_map<std::string, core::graphics::FrameBufferAttachment> m_outputIDs;
    std::unordered_map<std::string, core::UniformID> m_uniformIDs;
    std::unordered_map<std::string, core::UniformBlockID> m_uniformBlockIDs;
    std::unordered_map<std::string, core::ShaderStorageBlockID> m_shaderStorageBlockIDs;

    static std::weak_ptr<RendererBase> s_current;
};

} // namespace graphics
} // namespace core
} // namespace simplex

#endif // CORE_GRAPHICSRENDERERBASEPRIVATE_H
