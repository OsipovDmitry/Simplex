#ifndef CORE_TEXTURESLOADER_H
#define CORE_TEXTURESLOADER_H

#include <core/forwarddecl.h>
#include <core/resourceloader.h>

namespace simplex
{
namespace core
{

class TexturesLoaderPrivate;
class CORE_SHARED_EXPORT TexturesLoader : public ResourceLoaderBase<graphics::ITexture>
{
    NONCOPYBLE(TexturesLoader)
public:
    TexturesLoader(const std::string&, const std::shared_ptr<graphics::RendererBase>&);
    ~TexturesLoader() override;

    using ResourceLoaderBase<graphics::ITexture>::loadOrGet;
    std::shared_ptr<graphics::ITexture> loadOrGet(const std::shared_ptr<const utils::Image>&);

    std::shared_ptr<graphics::ITexture> loadOrGetDefaultIBLEnvironmentTexture();
    std::shared_ptr<graphics::ITexture> loadOrGetDefaultIBLBRDFLutTexture();
    std::shared_ptr<graphics::ITexture> loadOrGetDefaultIBLDiffuseTexture();
    std::shared_ptr<graphics::ITexture> loadOrGetDefaultIBLSpecularTexture();

private:
    std::unique_ptr<TexturesLoaderPrivate> m_;
};

}
}

#endif // CORE_TEXTURESLOADER_H
