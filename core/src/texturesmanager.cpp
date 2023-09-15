#include <utils/logger.h>
#include <utils/image.h>

#include <core/texturesmanager.h>

#include "texturesmanagerprivate.h"

namespace simplex
{
namespace core
{

TexturesManager::TexturesManager(std::shared_ptr<graphics::IRenderer> renderer)
    : m_(std::make_unique<TexturesManagerPrivate>(renderer))
{
}

TexturesManager::~TexturesManager() = default;

std::shared_ptr<graphics::ITexture> TexturesManager::loadOrGetTexture(const std::filesystem::path &imageFilename,
                                                                      graphics::PixelInternalFormat internalFormat,
                                                                      uint32_t numLevels,
                                                                      bool genMipmaps,
                                                                      const std::string &key)
{
    auto name = key;
    if (name == "autogen")
    {
        name = imageFilename.string() + std::to_string(graphics::castFromPixelInternalFormat(internalFormat)) + std::to_string(numLevels);
        // no need to put "genMipmaps" to "name" because it can be changed after loading by user
    }

    if (auto it = m_->resources().find(name); it != m_->resources().end())
    {
        return it->second;
    }

    std::shared_ptr<utils::Image> image;
    if (imageFilename.extension() == ".json")
    {

    }
    else
        image = utils::Image::loadFromFile(imageFilename);

    auto texture = m_->renderer()->createTexture2D(image, internalFormat, numLevels, genMipmaps);
    m_->resources()[name] = texture;

    return texture;
}

}
}
