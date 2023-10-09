#ifndef CORE_TEXTURESMANAGER_H
#define CORE_TEXTURESMANAGER_H

#include <memory>
#include <filesystem>

#include <utils/noncopyble.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/igraphicsrenderer.h>

namespace simplex
{
namespace core
{

class TexturesManagerPrivate;
class CORE_SHARED_EXPORT TexturesManager
{
    NONCOPYBLE(TexturesManager)
public:
    TexturesManager(std::shared_ptr<graphics::IRenderer>);
    ~TexturesManager();

    // internalFormat, numLevels, genMipmaps are ignored for .json decription textures
    // they can be set in the file itself
    std::shared_ptr<graphics::ITexture> loadOrGetTexture(const std::filesystem::path&,
                                                         graphics::PixelInternalFormat = graphics::PixelInternalFormat::Undefined,
                                                         uint32_t numLevels = 0,
                                                         bool genMipmaps = true,
                                                         const std::string& = "autogen");

    std::shared_ptr<graphics::ITexture> loadOrGetDefaultIBLEnvironmentTexture();
    std::shared_ptr<graphics::ITexture> loadOrGetDefaultIBLBRDFLutTexture();
    std::shared_ptr<graphics::ITexture> loadOrGetDefaultIBLDiffuseTexture();
    std::shared_ptr<graphics::ITexture> loadOrGetDefaultIBLSpecularTexture();

private:
    std::unique_ptr<TexturesManagerPrivate> m_;
};

}
}

#endif // CORE_TEXTURESMANAGER_H
