#ifndef CORE_PROGRAMSMANAGER_H
#define CORE_PROGRAMSMANAGER_H

#include <memory>
#include <filesystem>

#include <utils/noncopyble.h>
#include <utils/forwarddecl.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class ProgramsManagerPrivate;
class CORE_SHARED_EXPORT ProgramsManager
{
    NONCOPYBLE(ProgramsManager)
public:
    ProgramsManager(std::shared_ptr<graphics::IRenderer>);
    ~ProgramsManager();

    std::shared_ptr<graphics::IRenderProgram> loadOrGetRenderProgram(const std::filesystem::path&,
                                                                     const std::filesystem::path&,
                                                                     const utils::ShaderDefines&,
                                                                     const std::string& = "autogen");

    std::shared_ptr<graphics::IRenderProgram> loadOrGetRenderProgram(const std::filesystem::path&,
                                                                     const std::filesystem::path&,
                                                                     const std::filesystem::path&,
                                                                     const utils::ShaderDefines&,
                                                                     const std::string& = "autogen");

    std::shared_ptr<graphics::IComputeProgram> loadOrGetComputeProgram(const std::filesystem::path&,
                                                                       const utils::ShaderDefines&,
                                                                       const std::string& = "autogen");

    std::shared_ptr<graphics::IRenderProgram> loadOrGetOpaqueGeometryPassRenderProgram(const utils::VertexAttributesSet&,
                                                                                       const std::unordered_set<PBRComponent>&);

    std::shared_ptr<graphics::IRenderProgram> loadOrGetTransparentGeometryPassRenderProgram(const utils::VertexAttributesSet&,
                                                                                            const std::unordered_set<PBRComponent>&);

    std::shared_ptr<graphics::IComputeProgram> loadOrGetOITClearPassComputeProgram();
    std::shared_ptr<graphics::IComputeProgram> loadOrGetOITSortNodesPassComputeProgram();

    std::shared_ptr<graphics::IRenderProgram> loadOrGetShadowRenderProgram(const utils::VertexAttributesSet&,
                                                                           const std::unordered_set<PBRComponent>&,
                                                                           LightShadingMode);

    std::shared_ptr<graphics::IRenderProgram> loadOrGetStencilPassRenderProgram(const utils::VertexAttributesSet&,
                                                                                LightType);

    std::shared_ptr<graphics::IRenderProgram> loadOrGetLightPassRenderProgram(const utils::VertexAttributesSet&,
                                                                              const std::unordered_set<LightComponent>&,
                                                                              LightType,
                                                                              LightShadingMode);

    std::shared_ptr<graphics::IRenderProgram> loadOrGetBackgroundPassRenderProgram(const utils::VertexAttributesSet&,
                                                                                   const std::unordered_set<BackgroundComponent>&);

    std::shared_ptr<graphics::IRenderProgram> loadOrGetForegroundPassRenderProgram(const utils::VertexAttributesSet&);

    std::shared_ptr<graphics::IRenderProgram> loadOrGetFinalPassRenderProgram(const utils::VertexAttributesSet&);

    std::shared_ptr<graphics::IRenderProgram> loadOrGetPostprocessPassRenderProgram(const utils::VertexAttributesSet&);

private:
    std::unique_ptr<ProgramsManagerPrivate> m_;
};

}
}

#endif // CORE_PROGRAMSMANAGER_H
