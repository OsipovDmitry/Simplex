#ifndef LOADER_ASSIMP_LOADER_H
#define LOADER_ASSIMP_LOADER_H

#include <core/resourceloader.h>
#include <core/scenerepresentation.h>

#include <scenes_loader_assimp/scenesloaderassimpglobal.h>

namespace simplex
{
namespace scenes_loader_assimp
{

class SCENES_LOADER_ASSIMP_GLOBAL_SHARED_EXPORT AssimpScenesLoader : public core::LoaderBase<core::SceneRepresentation>
{
public:
    AssimpScenesLoader();
    ~AssimpScenesLoader() override;

    std::shared_ptr<core::SceneRepresentation> loadResource(const std::filesystem::path& absoluteFileName) const override;
};

}
}

#endif // LOADER_ASSIMP_LOADER_H
