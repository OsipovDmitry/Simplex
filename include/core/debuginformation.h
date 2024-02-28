#ifndef CORE_DEBUGINFORMATION_H
#define CORE_DEBUGINFORMATION_H

#include <string>
#include <vector>

namespace simplex
{
namespace core
{
namespace debug
{

struct CameraInformation
{
    std::string cameraName;
    uint32_t numOpaqueDrawablesRendered;
    uint32_t numTransparentDrawablesRendererd;
    uint32_t numTrasparentPixelsRendered;
    uint32_t numLightsRendered;
};

struct SceneInformation
{
    std::string sceneName;
    std::vector<CameraInformation> camerasInformation;
};

struct GraphicsEngineInformation
{
    std::string graphicsEngineName;
    std::vector<SceneInformation> scenesInformation;
};

}
}
}

#endif // CORE_DEBUGINFORMATION_H
