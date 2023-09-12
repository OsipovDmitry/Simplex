#ifndef CORE_RENDERPROGRAMSMANAGER_H
#define CORE_RENDERPROGRAMSMANAGER_H

#include "core/igraphicsrenderer.h"
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

class RenderProgramsManagerPrivate;
class CORE_SHARED_EXPORT RenderProgramsManager
{
    NONCOPYBLE(RenderProgramsManager)
public:
    RenderProgramsManager(std::shared_ptr<graphics::IRenderer>);
    ~RenderProgramsManager();

    std::shared_ptr<graphics::IRenderProgram> loadOrGetRenderProgram(const std::filesystem::path&,
                                                                     const std::filesystem::path&,
                                                                     const utils::ShaderDefines&,
                                                                     const std::string& = "autogen");

    std::shared_ptr<graphics::IComputeProgram> loadOrGetComputeProgram(const std::filesystem::path&,
                                                                       const utils::ShaderDefines&,
                                                                       const std::string& = "autogen");

    std::shared_ptr<graphics::IRenderProgram> loadOrGetOpaqueGeometryPassRenderProgram(const utils::VertexAttributesSet&,
                                                                                       const graphics::PBRComponentsSet&);

    std::shared_ptr<graphics::IRenderProgram> loadOrGetTransparentGeometryPassRenderProgram(const utils::VertexAttributesSet&,
                                                                                            const graphics::PBRComponentsSet&);

    std::shared_ptr<graphics::IComputeProgram> loadOrGetOITClearPassComputeProgram();
    std::shared_ptr<graphics::IComputeProgram> loadOrGetOITSortNodesPassComputeProgram();

    std::shared_ptr<graphics::IRenderProgram> loadOrGetFinalPassRenderProgram(const utils::VertexAttributesSet&);

private:
    std::unique_ptr<RenderProgramsManagerPrivate> m_;
};

}
}

#endif // CORE_RENDERPROGRAMSMANAGER_H
