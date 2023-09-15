#include <utils/mesh.h>

#include <core/igraphicsrenderer.h>

#include "programsmanagerprivate.h"

namespace simplex
{
namespace core
{

const std::string ProgramsManagerPrivate::s_opaqueGeometryPassRenderProgramName = "OpaqueGeometryPass";
const std::string ProgramsManagerPrivate::s_transparentGeometryPassRenderProgramName = "TransparentGeometryPass";
const std::string ProgramsManagerPrivate::s_OITClearPassComputeProgramName = "OITClearPass";
const std::string ProgramsManagerPrivate::s_OITSortNodesPassComputeProgramName = "OITSortNodesPass";
const std::string ProgramsManagerPrivate::s_finalPassRenderProgramName = "FinalPass";

const std::filesystem::path ProgramsManagerPrivate::s_geometryPassVertexShaderPath = "D:/res/shaders/deferred_geometry_pass.vert";
const std::filesystem::path ProgramsManagerPrivate::s_opaqueGeometryPassFragmnetShaderPath = "D:/res/shaders/deferred_opaque_geometry_pass.frag";
const std::filesystem::path ProgramsManagerPrivate::s_transparentGeometryPassFragmnetShaderPath = "D:/res/shaders/deferred_transparent_geometry_pass.frag";
const std::filesystem::path ProgramsManagerPrivate::s_OITClearPassComputeShaderPath = "D:/res/shaders/oit_clear.comp";
const std::filesystem::path ProgramsManagerPrivate::s_OITSortNodesPassComputeShaderPath = "D:/res/shaders/oit_sort.comp";
const std::filesystem::path ProgramsManagerPrivate::s_finalPassVertexShaderPath = "D:/res/shaders/final.vert";
const std::filesystem::path ProgramsManagerPrivate::s_finalPassFragmnetShaderPath = "D:/res/shaders/final.frag";

ProgramsManagerPrivate::ProgramsManagerPrivate(std::shared_ptr<graphics::IRenderer> renderer)
    : m_renderer(renderer)
{
}

ProgramsManagerPrivate::~ProgramsManagerPrivate() = default;

std::shared_ptr<graphics::IRenderer> &ProgramsManagerPrivate::renderer()
{
    return m_renderer;
}

std::unordered_map<std::string, std::shared_ptr<graphics::IProgram>> &ProgramsManagerPrivate::resources()
{
    return m_resources;
}

void ProgramsManagerPrivate::prepareVertexAttributesDefines(const utils::VertexAttributesSet &attribsSet,
                                                            utils::ShaderDefines &defines)
{
    static const std::array<std::string, utils::numElementsVertexAttribute()> s_table {
        "POSITION_ATTRIBUTE",
        "NORMAL_ATTRIBUTE",
        "TEX_COORDS_ATTRIBUTE",
        "BONES_IDS_ATTRIBUTE",
        "BONES_WEIGHTS_ATTRIBUTE",
        "TANGENT_ATTRIBUTE",
        "COLOR_ATTRIBUTE"
    };

    for (uint16_t i = 0; i < utils::numElementsVertexAttribute(); ++i)
        if (attribsSet.count(utils::castToVertexAttribute(i)))
            defines.insert({s_table[i], std::to_string(i)});
}

uint16_t ProgramsManagerPrivate::prepareVertexAttributesKey(const utils::VertexAttributesSet &attribsSet,
                                                            VertexAttributesKey &attribsKey,
                                                            uint16_t firstKeyIndex)
{
    for (uint16_t i = 0; i < utils::numElementsVertexAttribute(); ++i)
        attribsKey.set(firstKeyIndex + i, attribsSet.count(utils::castToVertexAttribute(i)));
    return firstKeyIndex + utils::numElementsVertexAttribute();
}

void ProgramsManagerPrivate::preparePBRComponentsDefines(const graphics::PBRComponentsSet &PBRComponentsSet,
                                                         utils::ShaderDefines &defines)
{
    static const std::array<std::string, graphics::numElementsPBRComponent()> s_table {
        "HAS_BASE_COLOR",
        "HAS_METALNESS",
        "HAS_ROUGHNESS",
        "HAS_BASE_COLOR_MAP",
        "HAS_METALNESS_MAP",
        "HAS_ROUGHNESS_MAP",
        "HAS_NORMAL_MAP",
    };

    for (uint16_t i = 0; i < graphics::numElementsPBRComponent(); ++i)
        if (PBRComponentsSet.count(graphics::castToPBRComponent(i)))
            defines.insert({s_table[i], ""});
}

uint16_t ProgramsManagerPrivate::preparePBRComponentsKey(const graphics::PBRComponentsSet &PBRComponentsSet,
                                                         VertexAttributesKey &attribsKey,
                                                         uint16_t firstKeyIndex)
{
    for (uint16_t i = 0; i < graphics::numElementsPBRComponent(); ++i)
        attribsKey.set(firstKeyIndex + i, PBRComponentsSet.count(graphics::castToPBRComponent(i)));
    return firstKeyIndex + graphics::numElementsPBRComponent();
}

void ProgramsManagerPrivate::prepareDefinesAndKeyForGeometryPassRenderProgram(const utils::VertexAttributesSet& attribsSet,
                                                                              const graphics::PBRComponentsSet &PBRComponentsSet,
                                                                              utils::ShaderDefines &defines,
                                                                              VertexAttributesKey &key)
{
    prepareVertexAttributesDefines(attribsSet, defines);
    preparePBRComponentsDefines(PBRComponentsSet, defines);

    uint16_t bit = 0u;
    bit = prepareVertexAttributesKey(attribsSet, key, bit);
    bit = preparePBRComponentsKey(PBRComponentsSet, key, bit);
}

const std::string &ProgramsManagerPrivate::opaqueGeometryPassRenderProgramName()
{
    return s_opaqueGeometryPassRenderProgramName;
}

const std::string &ProgramsManagerPrivate::transparentGeometryPassRenderProgramName()
{
    return s_transparentGeometryPassRenderProgramName;
}

const std::string &ProgramsManagerPrivate::OITClearPassComputeProgramName()
{
    return s_OITClearPassComputeProgramName;
}

const std::string &ProgramsManagerPrivate::OITSortNodesPassComputeProgramName()
{
    return s_OITSortNodesPassComputeProgramName;
}

const std::string &ProgramsManagerPrivate::finalPassRenderProgramName()
{
    return s_finalPassRenderProgramName;
}

const std::filesystem::path &ProgramsManagerPrivate::geometryPassVertexShaderPath()
{
    return s_geometryPassVertexShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::opaqueGeometryPassFragmnetShaderPath()
{
    return s_opaqueGeometryPassFragmnetShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::transparentGeometryPassFragmnetShaderPath()
{
    return s_transparentGeometryPassFragmnetShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::OITClearPassComputeShaderPath()
{
    return s_OITClearPassComputeShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::OITSortNodesPassComputeShaderPath()
{
    return s_OITSortNodesPassComputeShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::finalPassVertexShaderPath()
{
    return s_finalPassVertexShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::finalPassFragmnetShaderPath()
{
    return s_finalPassFragmnetShaderPath;
}

}
}
