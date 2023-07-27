#ifndef CORE_RENDERPROGRAMSMANAGER_H
#define CORE_RENDERPROGRAMSMANAGER_H

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

    std::shared_ptr<graphics::IRenderProgram> loadOrGet(const std::filesystem::path&,
                                                        const std::filesystem::path&,
                                                        const utils::ShaderDefines& = {});

private:
    std::unique_ptr<RenderProgramsManagerPrivate> m_;
};

}
}

#endif // CORE_RENDERPROGRAMSMANAGER_H
