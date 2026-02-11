#include <core/scenesloader.h>

#include "scenesloaderprivate.h"

namespace simplex
{
namespace core
{

ScenesLoader::ScenesLoader(const std::string& name)
    : ResourceLoaderBase<SceneRepresentation>(name)
    , m_(std::make_unique<ScenesLoaderPrivate>())
{
}

ScenesLoader::~ScenesLoader() = default;

}
}
