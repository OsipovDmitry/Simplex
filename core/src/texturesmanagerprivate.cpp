#include <utils/image.h>

#include <core/igraphicsrenderer.h>

#include "texturesmanagerprivate.h"

namespace simplex
{
namespace core
{

TexturesManagerPrivate::TexturesManagerPrivate(std::shared_ptr<graphics::IRenderer> renderer)
    : m_renderer(renderer)
{
}

TexturesManagerPrivate::~TexturesManagerPrivate() = default;

std::shared_ptr<graphics::IRenderer> &TexturesManagerPrivate::renderer()
{
    return m_renderer;
}

std::unordered_map<std::string, std::shared_ptr<graphics::ITexture>> &TexturesManagerPrivate::resources()
{
    return m_resources;
}

}
}
