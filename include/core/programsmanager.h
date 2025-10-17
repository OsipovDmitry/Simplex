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

private:
    std::unique_ptr<ProgramsManagerPrivate> m_;
};

}
}

#endif // CORE_PROGRAMSMANAGER_H
