#ifndef CORE_RESOURCES_H
#define CORE_RESOURCES_H

#include <filesystem>

namespace simplex
{
namespace core
{
namespace resources
{

static const std::filesystem::path DefaultIBLEnvironmentMapPath = "./resources/textures/ibl/environment.json";
static const std::filesystem::path DefaultIBLBRDFLutMapPath = "./resources/textures/ibl/brdf.json";
static const std::filesystem::path DefaultIBLDiffuseMapPath = "./resources/textures/ibl/irradiance.json";
static const std::filesystem::path DefaultIBLSpecularMapPath = "./resources/textures/ibl/radiance.json";

static const std::filesystem::path BuildBackgroundsCommandsBufferPassComputeShaderPath = "./resources/shaders/build_backgrounds_commands_buffer_pass.comp";
static const std::filesystem::path BuildDrawDataCommandsBufferPassComputeShaderPath = "./resources/shaders/build_drawdata_commands_buffer_pass.comp";
static const std::filesystem::path RenderOpaqueDrawDataPassVertexShaderPath = "./resources/shaders/render_opaque_drawdata_geometry_pass.vert";
static const std::filesystem::path RenderOpaqueDrawDataPassFragmentShaderPath = "./resources/shaders/render_opaque_drawdata_geometry_pass.frag";

}
}
}

#endif // CORE_RESOURCES_H