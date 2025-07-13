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
    ProgramsManager(const std::shared_ptr<graphics::RendererBase>&);
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

    std::shared_ptr<graphics::IRenderProgram> loadOrGetGeometryPassRenderProgram(const utils::VertexAttributeSet&,
                                                                                 const UniformCollection&,
                                                                                 bool);

    std::shared_ptr<graphics::IRenderProgram> loadOrGetStencilPassRenderProgram(const utils::VertexAttributeSet&,
                                                                                LightType);

    std::shared_ptr<graphics::IRenderProgram> loadOrGetLightPassRenderProgram(const utils::VertexAttributeSet&,
                                                                              const UniformCollection&,
                                                                              LightType,
                                                                              ShadingMode,
                                                                              ShadingFilter,
                                                                              SSAOMode);

    std::shared_ptr<graphics::IComputeProgram> loadOrGetOITClearPassComputeProgram();
    std::shared_ptr<graphics::IComputeProgram> loadOrGetOITSortNodesPassComputeProgram();

    std::shared_ptr<graphics::IRenderProgram> loadOrGetSSAORenderProgram(const utils::VertexAttributeSet&,
                                                                         SSAOMode);

    std::shared_ptr<graphics::IRenderProgram> loadOrGetBlurRenderProgram(const utils::VertexAttributeSet&);
    std::shared_ptr<graphics::IComputeProgram> loadOrGetBlurComputeProgram(graphics::TextureType,
                                                                           graphics::PixelInternalFormat);
    std::shared_ptr<graphics::IRenderProgram> loadOrGetBilaterialBlurRenderProgram(const utils::VertexAttributeSet&);

    std::shared_ptr<graphics::IRenderProgram> loadOrGetShadowRenderProgram(const utils::VertexAttributeSet&,
                                                                           const UniformCollection&,
                                                                           ShadingMode,
                                                                           ShadingFilter);

    std::shared_ptr<graphics::IRenderProgram> loadOrGetBackgroundPassRenderProgram(const utils::VertexAttributeSet&,
                                                                                   const UniformCollection&);

    std::shared_ptr<graphics::IRenderProgram> loadOrGetFinalPassRenderProgram(const utils::VertexAttributeSet&);

    std::shared_ptr<graphics::IRenderProgram> loadOrGetPostprocessPassRenderProgram(const utils::VertexAttributeSet&);

private:
    std::unique_ptr<ProgramsManagerPrivate> m_;
};

}
}

#endif // CORE_PROGRAMSMANAGER_H
