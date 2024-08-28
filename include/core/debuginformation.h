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
    uint32_t numOpaqueDrawablesRendered = 0u;
    uint32_t numTransparentDrawablesRendered = 0u;
    uint32_t numFragmentsRendered = 0u;
    uint32_t numLightsRendered = 0u;
};

struct SceneInformation
{
    std::string sceneName;
    std::vector<CameraInformation> camerasInformation;
};

struct Information
{
    std::vector<SceneInformation> scenesInformation;
};

}
}
}

#endif // CORE_DEBUGINFORMATION_H
