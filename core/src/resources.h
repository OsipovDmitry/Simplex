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

static const std::filesystem::path FrustumCullingPassComputeShaderPath = "./resources/shaders/frustum_culling_pass.comp";
static const std::filesystem::path ClearOITIndicesImagePassComputeShaderPath = "./resources/shaders/clear_oit_indices_image_pass.comp";
static const std::filesystem::path RenderDrawDataGeometryPassVertexShaderPath = "./resources/shaders/render_drawdata_geometry_pass.vert";
static const std::filesystem::path RenderOpaqueDrawDataGeometryPassFragmentShaderPath = "./resources/shaders/render_opaque_drawdata_geometry_pass.frag";
static const std::filesystem::path RenderTransparentDrawDataGeometryPassFragmentShaderPath = "./resources/shaders/render_transparent_drawdata_geometry_pass.frag";
static const std::filesystem::path SortOITNodesPassComputeShaderPath = "./resources/shaders/sort_oit_nodes_pass.comp";
static const std::filesystem::path RenderBackgroundPassVertexShaderPath = "./resources/shaders/render_background_pass.vert";
static const std::filesystem::path RenderBackgroundPassFragmentShaderPath = "./resources/shaders/render_background_pass.frag";
static const std::filesystem::path BuildLightsCommandsBufferPassComputeShaderPath = "./resources/shaders/build_lights_commands_buffer_pass.comp";
static const std::filesystem::path RenderDrawDataStencilPassVertexShaderPath = "./resources/shaders/render_drawdata_stencil_pass.vert";
static const std::filesystem::path RenderDrawDataStencilPassFragmentShaderPath = "./resources/shaders/render_drawdata_stencil_pass.frag";
static const std::filesystem::path BlendPassVertexShaderPath = "./resources/shaders/blend_pass.vert";
static const std::filesystem::path BlendPassFragmentShaderPath = "./resources/shaders/blend_pass.frag";

}
}
}

#endif // CORE_RESOURCES_H