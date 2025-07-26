#ifndef CORE_RESOURCES_H
#define CORE_RESOURCES_H

#include <filesystem>

namespace simplex
{
namespace core
{
namespace resources
{

static const std::filesystem::path DefaultIBLEnvironmentMapPath = "C:/res/textures/ibl/environment.json";
static const std::filesystem::path DefaultIBLBRDFLutMapPath = "C:/res/textures/ibl/brdf.json";
static const std::filesystem::path DefaultIBLDiffuseMapPath = "C:/res/textures/ibl/irradiance.json";
static const std::filesystem::path DefaultIBLSpecularMapPath = "C:/res/textures/ibl/radiance.json";

}
}
}

#endif // CORE_RESOURCES_H