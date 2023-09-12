#ifndef CORE_RENDERPROGRAMSMANAGERPRIVATE_H
#define CORE_RENDERPROGRAMSMANAGERPRIVATE_H

#include <memory>
#include <unordered_map>
#include <string>
#include <bitset>
#include <filesystem>

#include <utils/forwarddecl.h>
#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class RenderProgramsManagerPrivate
{
public:
    using VertexAttributesKey = std::bitset<32>;

    RenderProgramsManagerPrivate(std::shared_ptr<graphics::IRenderer>);
    ~RenderProgramsManagerPrivate();

    std::shared_ptr<graphics::IRenderer> &renderer();
    std::unordered_map<std::string, std::shared_ptr<graphics::IProgram>> &resources();

    static void prepareVertexAttributesDefines(const utils::VertexAttributesSet&,
                                               utils::ShaderDefines&);
    static uint16_t prepareVertexAttributesKey(const utils::VertexAttributesSet&,
                                               VertexAttributesKey&,
                                               uint16_t);

    static void preparePBRComponentsDefines(const graphics::PBRComponentsSet&,
                                            utils::ShaderDefines&);
    static uint16_t preparePBRComponentsKey(const graphics::PBRComponentsSet&,
                                            VertexAttributesKey&,
                                            uint16_t);

    static void prepareDefinesAndKeyForGeometryPassRenderProgram(const utils::VertexAttributesSet&,
                                                                 const graphics::PBRComponentsSet&,
                                                                 utils::ShaderDefines&,
                                                                 VertexAttributesKey&);

    static const std::string &opaqueGeometryPassRenderProgramName();
    static const std::string &transparentGeometryPassRenderProgramName();
    static const std::string &OITClearPassComputeProgramName();
    static const std::string &OITSortNodesPassComputeProgramName();
    static const std::string &finalPassRenderProgramName();

    static const std::filesystem::path &geometryPassVertexShaderPath();
    static const std::filesystem::path &opaqueGeometryPassFragmnetShaderPath();
    static const std::filesystem::path &transparentGeometryPassFragmnetShaderPath();
    static const std::filesystem::path &OITClearPassComputeShaderPath();
    static const std::filesystem::path &OITSortNodesPassComputeShaderPath();
    static const std::filesystem::path &finalPassVertexShaderPath();
    static const std::filesystem::path &finalPassFragmnetShaderPath();

private:
    std::shared_ptr<graphics::IRenderer> m_renderer;
    std::unordered_map<std::string, std::shared_ptr<graphics::IProgram>> m_resources;

    static const std::string s_opaqueGeometryPassRenderProgramName;
    static const std::string s_transparentGeometryPassRenderProgramName;
    static const std::string s_OITClearPassComputeProgramName;
    static const std::string s_OITSortNodesPassComputeProgramName;
    static const std::string s_finalPassRenderProgramName;

    static const std::filesystem::path s_geometryPassVertexShaderPath;
    static const std::filesystem::path s_opaqueGeometryPassFragmnetShaderPath;
    static const std::filesystem::path s_transparentGeometryPassFragmnetShaderPath;
    static const std::filesystem::path s_OITClearPassComputeShaderPath;
    static const std::filesystem::path s_OITSortNodesPassComputeShaderPath;
    static const std::filesystem::path s_finalPassVertexShaderPath;
    static const std::filesystem::path s_finalPassFragmnetShaderPath;
};

}
}

#endif // CORE_RENDERPROGRAMSMANAGERPRIVATE_H
