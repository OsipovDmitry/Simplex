#ifndef CORE_PROGRAMSLOADER_H
#define CORE_PROGRAMSLOADER_H

#include <core/forwarddecl.h>
#include <core/resourceloader.h>

namespace simplex
{
namespace core
{

class ProgramsLoaderPrivate;
class CORE_SHARED_EXPORT ProgramsLoader : public ResourceLoaderBase<graphics::IProgram>
{
    NONCOPYBLE(ProgramsLoader)
public:
    ProgramsLoader(const std::string&, const std::shared_ptr<graphics::RendererBase>&);
    ~ProgramsLoader();

    std::shared_ptr<graphics::IRenderProgram> loadOrGetRenderProgram(const std::filesystem::path&,
                                                                     const std::filesystem::path&,
                                                                     const utils::ShaderDefines&);

    std::shared_ptr<graphics::IRenderProgram> loadOrGetRenderProgram(const std::filesystem::path&,
                                                                     const std::filesystem::path&,
                                                                     const std::filesystem::path&,
                                                                     const utils::ShaderDefines&);

    std::shared_ptr<graphics::IComputeProgram> loadOrGetComputeProgram(const std::filesystem::path&,
                                                                       const utils::ShaderDefines&);

private:
    std::unique_ptr<ProgramsLoaderPrivate> m_;
};

}
}

#endif // CORE_PROGRAMSLOADER_H
