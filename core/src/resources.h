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

static const std::filesystem::path InitializeCameraPassComputeShaderPath = "./resources/shaders/initialize_camera_pass.comp";
static const std::filesystem::path CullDrawDataPassComputeShaderPath = "./resources/shaders/cull_draw_data_pass.comp";
static const std::filesystem::path CollectSkeletalAnimatedDataToUpdatePassComputeShaderPath =
    "./resources/shaders/collect_skeletal_animated_data_to_update_pass.comp";
static const std::filesystem::path UpdateCameraPassComputeShaderPath = "./resources/shaders/update_camera_pass.comp";
static const std::filesystem::path PrepareBonesTransformsDataCalculateCommandPassComputeShaderPath =
    "./resources/shaders/prepare_bones_transforms_data_calculate_command_pass.comp";
static const std::filesystem::path CalculateBonesTransformsDataPassComputeShaderPath =
    "./resources/shaders/calculate_bones_transforms_data_pass.comp";
static const std::filesystem::path BuildClusterPassComputeShaderPath = "./resources/shaders/build_cluster_pass.comp";
static const std::filesystem::path ClearOITNodeIDImagePassComputeShaderPath =
    "./resources/shaders/clear_oit_node_id_image_pass.comp";
static const std::filesystem::path RenderDrawDataPassVertexShaderPath = "./resources/shaders/render_draw_data_pass.vert";
static const std::filesystem::path RenderOpaqueDrawDataPassFragmentShaderPath =
    "./resources/shaders/render_opaque_draw_data_pass.frag";
static const std::filesystem::path RenderTransparentDrawDataPassFragmentShaderPath =
    "./resources/shaders/render_transparent_draw_data_pass.frag";
static const std::filesystem::path SortOITNodesPassComputeShaderPath = "./resources/shaders/sort_oit_nodes_pass.comp";
static const std::filesystem::path ClusterLightPassComputeShaderPath = "./resources/shaders/cluster_light_pass.comp";
static const std::filesystem::path PrepareShadowDataCullCommandPassComputeShaderPath =
    "./resources/shaders/prepare_shadow_data_cull_command_pass.comp";
static const std::filesystem::path PrepareShadowMapBlurCommandsComputeShaderPath =
    "./resources/shaders/prepare_shadow_map_blur_commands_pass.comp";
static const std::filesystem::path CullShadowDataPassComputeShaderPath = "./resources/shaders/cull_shadow_data_pass.comp";
static const std::filesystem::path RenderShadowDataPassVertexShaderPath = "./resources/shaders/render_shadow_data_pass.vert";
static const std::filesystem::path RenderShadowDataPassGeometryShaderPath = "./resources/shaders/render_shadow_data_pass.geom";
static const std::filesystem::path RenderOpaqueShadowDataPassFragmentShaderPath =
    "./resources/shaders/render_opaque_shadow_data_pass.frag";
static const std::filesystem::path RenderTransparentShadowDataPassFragmentShaderPath =
    "./resources/shaders/render_transparent_shadow_data_pass.frag";
static const std::filesystem::path BlurShadowMapPassVertexShaderPath = "./resources/shaders/blur_shadow_map_pass.vert";
static const std::filesystem::path BlurShadowMapPassGeometryShaderPath = "./resources/shaders/blur_shadow_map_pass.geom";
static const std::filesystem::path BlurShadowMapPassFragmentShaderPath = "./resources/shaders/blur_shadow_map_pass.frag";
static const std::filesystem::path RenderBackgroundPassVertexShaderPath = "./resources/shaders/render_background_pass.vert";
static const std::filesystem::path RenderBackgroundPassFragmentShaderPath = "./resources/shaders/render_background_pass.frag";
static const std::filesystem::path BlendPassVertexShaderPath = "./resources/shaders/blend_pass.vert";
static const std::filesystem::path BlendPassFragmentShaderPath = "./resources/shaders/blend_pass.frag";
static const std::filesystem::path FinalPassVertexShaderPath = "./resources/shaders/final_pass.vert";
static const std::filesystem::path FinalPassFragmentShaderPath = "./resources/shaders/final_pass.frag";

} // namespace resources
} // namespace core
} // namespace simplex

#endif // CORE_RESOURCES_H