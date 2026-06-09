#include <utils/clipspace.h>
#include <utils/epsilon.h>
#include <utils/range.h>

#include <core/graphicsengine.h>
#include <core/settings.h>

namespace simplex
{
namespace core
{
namespace settings
{

namespace Conversions
{

static utils::ClipSpaceType string2ClipSpaceType(const std::string& value)
{
    static std::unordered_map<std::string, utils::ClipSpaceType> s_table{
        {"ortho", utils::ClipSpaceType::Ortho},
        {"perspective", utils::ClipSpaceType::Perspective}};

    auto it = s_table.find(value);
    return (it != s_table.end()) ? it->second : utils::ClipSpaceType::Perspective;
}

static DrawDataCullingAlgorithm string2DrawDataCullingAlgorithm(const std::string& value)
{
    static std::unordered_map<std::string, DrawDataCullingAlgorithm> s_table{
        {"disabled", DrawDataCullingAlgorithm::Disabled},
        {"superfast", DrawDataCullingAlgorithm::SuperFast},
        {"fast", DrawDataCullingAlgorithm::Fast},
        {"correct", DrawDataCullingAlgorithm::Correct}};

    auto it = s_table.find(value);
    return (it != s_table.end()) ? it->second : DrawDataCullingAlgorithm::SuperFast;
}

static ShadowDataCullingAlgorithm string2ShadowDataCullingAlgorithm(const std::string& value)
{
    static std::unordered_map<std::string, ShadowDataCullingAlgorithm> s_table{
        {"disabled", ShadowDataCullingAlgorithm::Disabled},
        {"superfast", ShadowDataCullingAlgorithm::SuperFast},
        {"fast", ShadowDataCullingAlgorithm::Fast},
        {"correct", ShadowDataCullingAlgorithm::Correct}};

    auto it = s_table.find(value);
    return (it != s_table.end()) ? it->second : ShadowDataCullingAlgorithm::SuperFast;
}

static SpotLightCullingAlgorithm string2SpotLightCullingAlgorithm(const std::string& value)
{
    static std::unordered_map<std::string, SpotLightCullingAlgorithm> s_table{
        {"superfast", SpotLightCullingAlgorithm::SuperFast},
        {"fast", SpotLightCullingAlgorithm::Fast},
        {"correct", SpotLightCullingAlgorithm::Correct}};

    auto it = s_table.find(value);
    return (it != s_table.end()) ? it->second : SpotLightCullingAlgorithm::SuperFast;
}

// SSAOMode string2SSAOMode(const std::string &value)
//{
//     static std::unordered_map<std::string, SSAOMode> s_table {
//         { "disabled", SSAOMode::Disabled },
//         { "opaque", SSAOMode::Opaque },
//         { "opaqueandtransparent", SSAOMode::OpaqueAndTransparent },
//         { "color", SSAOMode::Color }
//     };
//
//     auto it = s_table.find(value);
//     return (it != s_table.end()) ? it->second : SSAOMode::Color;
// }

} // namespace Conversions

Scene::Scene(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

Scene::~Scene() = default;

const std::string& Scene::sceneRootNodeName() const
{
    static const auto s_name = readString("RootNodeName", "SceneRootNode");
    return s_name;
}

const std::string& Scene::listenerNodeName() const
{
    static const auto s_name = readString("ListenerNodeName", "ListenerNode");
    return s_name;
}

Application::Application(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

Application::~Application() = default;

const std::string& Application::name() const
{
    static const auto s_name = readString("Name", "Simplex3DEngine");
    return s_name;
}

const Scene& Application::scene() const
{
    static const Scene s_scene(read("Scene"));
    return s_scene;
}

ClipSpace::ClipSpace(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

ClipSpace::~ClipSpace() = default;

float ClipSpace::orthoHeight() const
{
    static const auto s_orthoHeight = readSingle("OrthoHeight", 1.f);
    return s_orthoHeight;
}

float ClipSpace::perspectiveFOV() const
{
    static const auto s_perspectiveFOV = readSingle("PerspectiveFOV", 1.570796f);
    return s_perspectiveFOV;
}

utils::ClipSpaceType ClipSpace::type() const
{
    static auto s_type = utils::ClipSpaceType::Count;

    if (s_type == utils::ClipSpaceType::Count)
    {
        auto typeString = readString("Type", "perspective");
        for (auto& c : typeString)
            c = static_cast<char>(std::tolower(c));

        s_type = Conversions::string2ClipSpaceType(typeString);
    }

    return s_type;
}

Camera::Camera(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

Camera::~Camera() = default;

const ClipSpace& Camera::clipSpace() const
{
    static const ClipSpace s_clipsapce(read("ClipSpace"));
    return s_clipsapce;
}

const glm::u32vec3& Camera::clusterSize() const
{
    static const auto s_clusterSize = readUVec3("ClusterSize", glm::u32vec3(16u, 9u, 24u));
    return s_clusterSize;
}

Background::Background(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

Background::~Background() = default;

const glm::vec3& Background::environmentColor() const
{
    static const auto s_color = readVec3("EnvironmentColor", glm::vec3(1.f));
    return s_color;
}

float Background::blurPower() const
{
    static const auto s_blurPower = readSingle("BlurPower", 1.f);
    return s_blurPower;
}

Flat::Flat(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

Flat::~Flat() = default;

const glm::vec4& Flat::color() const
{
    static const auto s_color = readVec4("Color", glm::vec4(1.f));
    return s_color;
}

PBR::PBR(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

PBR::~PBR() = default;

float PBR::dielectricSpecular() const
{
    static const auto s_dielectricSpecular = readSingle("DielectricSpecular", .04f);
    return s_dielectricSpecular;
}

const glm::u32vec3& PBR::ORMSwizzleMask() const
{
    static const auto s_ORMSwizzleMask = readUVec3("ORMSwizzleMask", glm::u32vec3(0u));
    return s_ORMSwizzleMask;
}

const glm::vec4& PBR::baseColor() const
{
    static const auto s_baseColor = readVec4("BaseColor", glm::vec4(1.f));
    return s_baseColor;
}

float PBR::roughness() const
{
    static const auto s_roughness = readSingle("Roughness", 1.f);
    return s_roughness;
}

float PBR::metalness() const
{
    static const auto s_metalness = readSingle("Metalness", 1.f);
    return s_metalness;
}

float PBR::normalMapScale() const
{
    static const auto s_normalMapScale = readSingle("NormalMapScale", 1.f);
    return s_normalMapScale;
}

float PBR::occlusionMapStrength() const
{
    static const auto s_occlusionMapStrength = readSingle("OcclusionMapStrngth", 1.f);
    return s_occlusionMapStrength;
}

const glm::vec3& PBR::emission() const
{
    static const auto s_emissionColor = readVec3("Emission", glm::vec3(0.f));
    return s_emissionColor;
}

IBL::IBL(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

IBL::~IBL() = default;

float IBL::contribution() const
{
    static const auto s_contribution = readSingle("Contribution", .4f);
    return s_contribution;
}

Shadow::Shadow(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

Shadow::~Shadow() = default;

uint32_t Shadow::atlasSize() const
{
    static const auto s_atlasSize = readUint("AtlasSize", 2048u);
    return s_atlasSize;
}

uint32_t Shadow::mapSize() const
{
    static const auto s_mapSize = readUint("MapSize", 512u);
    return s_mapSize;
}

float Shadow::blurSigma() const
{
    static const auto s_blurSigma = readSingle("BlurSigma", 3.f);
    return s_blurSigma;
}

float Shadow::lightBleedingAmount() const
{
    static const auto s_lightBleedingAmount = readSingle("LightBleedingAmount", .2f);
    return s_lightBleedingAmount;
}

SSAO::SSAO(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

SSAO::~SSAO() = default;

// SSAOMode SSAO::mode() const
//{
//     auto modeString = readString("Mode", "color");
//     for (auto &c : modeString)
//         c = static_cast<char>(std::tolower(c));
//
//     return Conversions::string2SSAOMode(modeString);
// }

float SSAO::viewportDenominator() const
{
    static const auto s_viewportDenominator = readSingle("ViewportDenominator", 1.f);
    return s_viewportDenominator;
}

float SSAO::contribution() const
{
    static const auto s_contribution = readSingle("Contribution", 1.f);
    return s_contribution;
}

uint32_t SSAO::kernelSize() const
{
    static const auto s_kernelSize = readUint("KernelSize", 16u);
    return s_kernelSize;
}

uint32_t SSAO::noiseSize() const
{
    static const auto s_noiseSize = readUint("NoiseSize", 4u);
    return s_noiseSize;
}

float SSAO::radius() const
{
    static const auto s_radius = readSingle("Radius", .3f);
    return s_radius;
}

Blur::Blur(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

Blur::~Blur() = default;

float Blur::sigma() const
{
    static const auto s_sigma = readSingle("Sigma", 2.f);
    return s_sigma;
}

OIT::OIT(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

OIT::~OIT() = default;

uint32_t OIT::nodesPerPixel() const
{
    static const auto s_nodesPerPixel = readUint("NodesPerPixel", 4u);
    return s_nodesPerPixel;
}

uint32_t OIT::maxNodes() const
{
    static const auto s_maxNodes = readUint("MaxNodes", 1920u * 1080u * 4u);
    return s_maxNodes;
}

NodeBoundingBox::NodeBoundingBox(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

NodeBoundingBox::~NodeBoundingBox() = default;

bool NodeBoundingBox::isEnabled() const
{
    static const auto s_isEnabled = readBool("IsEnabled", false);
    return s_isEnabled;
}

const glm::vec4& NodeBoundingBox::color() const
{
    static const auto s_color = readVec4("Color", glm::vec4(1.f, 0.f, 0.f, 1.f));
    return s_color;
}

DrawableNodeLocalBoundingBox::DrawableNodeLocalBoundingBox(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

DrawableNodeLocalBoundingBox::~DrawableNodeLocalBoundingBox() = default;

bool DrawableNodeLocalBoundingBox::isEnabled() const
{
    static const auto s_isEnabled = readBool("IsEnabled", false);
    return s_isEnabled;
}

const glm::vec4& DrawableNodeLocalBoundingBox::color() const
{
    static const auto s_color = readVec4("Color", glm::vec4(0.f, 1.f, 0.f, 1.f));
    return s_color;
}

DrawableBoundingBox::DrawableBoundingBox(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

DrawableBoundingBox::~DrawableBoundingBox() = default;

bool DrawableBoundingBox::isEnabled() const
{
    static const auto s_isEnabled = readBool("IsEnabled", false);
    return s_isEnabled;
}

const glm::vec4& DrawableBoundingBox::color() const
{
    static const auto s_color = readVec4("Color", glm::vec4(0.f, 0.f, 1.f, 1.f));
    return s_color;
}

LightNodeAreaBoundingBox::LightNodeAreaBoundingBox(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

LightNodeAreaBoundingBox::~LightNodeAreaBoundingBox() = default;

bool LightNodeAreaBoundingBox::isEnabled() const
{
    static const auto s_isEnabled = readBool("IsEnabled", false);
    return s_isEnabled;
}

const glm::vec4& LightNodeAreaBoundingBox::color() const
{
    static const auto s_color = readVec4("Color", glm::vec4(1.f, 1.f, 1.f, .2f));
    return s_color;
}

DebugRendering::DebugRendering(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

DebugRendering::~DebugRendering() = default;

const NodeBoundingBox& DebugRendering::nodeBoundingBox() const
{
    static const NodeBoundingBox s_nodeBoundingBox(read("NodeBoundingBox"));
    return s_nodeBoundingBox;
}

const DrawableNodeLocalBoundingBox& DebugRendering::drawableNodeLocalBoundingBox() const
{
    static const DrawableNodeLocalBoundingBox s_drawableNodeLocalBoundingBox(read("DrawableNodeLocalBoundingBox"));
    return s_drawableNodeLocalBoundingBox;
}

const DrawableBoundingBox& DebugRendering::drawableBoundingBox() const
{
    static const DrawableBoundingBox s_drawableBoundingBox(read("DrawableBoundingBox"));
    return s_drawableBoundingBox;
}

const LightNodeAreaBoundingBox& DebugRendering::lightNodeAreaBoundingBox() const
{
    static const LightNodeAreaBoundingBox s_lightNodeAreaBoundingBox(read("LightNodeAreaBoundingBox"));
    return s_lightNodeAreaBoundingBox;
}

Graphics::Graphics(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

Graphics::~Graphics() = default;

const utils::Range& Graphics::cullPlaneLimits() const
{
    static const auto s_cullPlaneLimits = utils::Range(readVec2("CullPlaneLimits", glm::vec2(.1f, 1000000.f)));
    return s_cullPlaneLimits;
}

DrawDataCullingAlgorithm Graphics::drawDataCullingAlgorithm() const
{
    static auto s_drawDataCullingAlgorithm = DrawDataCullingAlgorithm::Count;

    if (s_drawDataCullingAlgorithm == DrawDataCullingAlgorithm::Count)
    {
        auto algoString = readString("DrawDataCullingAlgorithm", "correct");
        for (auto& c : algoString)
            c = static_cast<char>(std::tolower(c));

        s_drawDataCullingAlgorithm = Conversions::string2DrawDataCullingAlgorithm(algoString);
    }

    return s_drawDataCullingAlgorithm;
}

ShadowDataCullingAlgorithm Graphics::shadowDataCullingAlgorithm() const
{
    static auto s_shadowDataCullingAlgorithm = ShadowDataCullingAlgorithm::Count;

    if (s_shadowDataCullingAlgorithm == ShadowDataCullingAlgorithm::Count)
    {
        auto algoString = readString("ShadowDataCullingAlgorithm", "correct");
        for (auto& c : algoString)
            c = static_cast<char>(std::tolower(c));

        s_shadowDataCullingAlgorithm = Conversions::string2ShadowDataCullingAlgorithm(algoString);
    }

    return s_shadowDataCullingAlgorithm;
}

SpotLightCullingAlgorithm Graphics::spotLightCullingAlgorithm() const
{
    static auto s_spotLightCullingAlgorithm = SpotLightCullingAlgorithm::Count;

    if (s_spotLightCullingAlgorithm == SpotLightCullingAlgorithm::Count)
    {
        auto algoString = readString("SpotLightCullingAlgorithm", "correct");
        for (auto& c : algoString)
            c = static_cast<char>(std::tolower(c));

        s_spotLightCullingAlgorithm = Conversions::string2SpotLightCullingAlgorithm(algoString);
    }

    return s_spotLightCullingAlgorithm;
}

const Camera& Graphics::camera() const
{
    static const Camera s_camera(read("Camera"));
    return s_camera;
}

const Background& Graphics::background() const
{
    static const Background s_background(read("Background"));
    return s_background;
}

const Flat& Graphics::flat() const
{
    static const Flat s_flat(read("Flat"));
    return s_flat;
}

const PBR& Graphics::pbr() const
{
    static const PBR s_PBR(read("PBR"));
    return s_PBR;
}

const IBL& Graphics::ibl() const
{
    static const IBL s_IBL(read("IBL"));
    return s_IBL;
}

const Shadow& Graphics::shadow() const
{
    static const Shadow s_shadow(read("Shadow"));
    return s_shadow;
}

const SSAO& Graphics::ssao() const
{
    static const SSAO s_SSAO(read("SSAO"));
    return s_SSAO;
}

const Blur& Graphics::blur() const
{
    static const Blur s_blur(read("Blur"));
    return s_blur;
}

const OIT& Graphics::oit() const
{
    static const OIT s_OIT(read("OIT"));
    return s_OIT;
}

const DebugRendering& Graphics::debugRendering() const
{
    static const DebugRendering s_debugRender(read("DebugRendering"));
    return s_debugRender;
}

Source::Source(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

Source::~Source() = default;

float Source::gain() const
{
    static const auto s_gain = readSingle("Gain", 1.f);
    return s_gain;
}

float Source::minGain() const
{
    static const auto s_minGain = readSingle("MinGain", 0.f);
    return s_minGain;
}

float Source::maxGain() const
{
    static const auto s_maxGain = readSingle("MaxGain", 1.f);
    return s_maxGain;
}

float Source::referenceDistance() const
{
    static const auto s_referenceDistance = readSingle("ReferenceDistance", 1.f);
    return s_referenceDistance;
}

float Source::rolloffFactor() const
{
    static const auto s_rolloffFactor = readSingle("RolloffFactor", 1.f);
    return s_rolloffFactor;
}

float Source::maxDistance() const
{
    static const auto s_maxDistance = readSingle("MaxDistance", utils::maximum<float>());
    return s_maxDistance;
}

float Source::pitch() const
{
    static const auto s_pitch = readSingle("Pitch", 1.f);
    return s_pitch;
}

Listener::Listener(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

Listener::~Listener() = default;

float Listener::gain() const
{
    static const auto s_gain = readSingle("Gain", 1.f);
    return s_gain;
}

Audio::Audio(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

Audio::~Audio() = default;

const Source& Audio::source() const
{
    static const Source s_source(read("Source"));
    return s_source;
}

const Listener& Audio::listener() const
{
    static const Listener s_listener(read("Listener"));
    return s_listener;
}

Physics::Physics(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

Physics::~Physics() = default;

const glm::vec3& Physics::gravity() const
{
    static const auto s_gravity(readVec3("Gravity", glm::vec3(0.f, -9.8f, 0.f)));
    return s_gravity;
}

uint32_t Physics::simulationTimeStep() const
{
    static const auto s_simulationTimeStep(readUint("SimulationTimeStep", 17u));
    return s_simulationTimeStep;
}

class SettingsPrivate
{
public:
    SettingsPrivate() {}
    ~SettingsPrivate() = default;

    static std::filesystem::path s_path;
};

std::filesystem::path SettingsPrivate::s_path = "./resources/settings/settings.json";

Settings::~Settings() = default;

const Application& Settings::application() const
{
    static const Application s_application(read("Application"));
    return s_application;
}

const Graphics& Settings::graphics() const
{
    static const Graphics s_graphics(read("Graphics"));
    return s_graphics;
}

const Audio& Settings::audio() const
{
    static const Audio s_audio(read("Audio"));
    return s_audio;
}

const Physics& Settings::physics() const
{
    static const Physics s_physics(read("Physics"));
    return s_physics;
}

void Settings::setPath(const std::filesystem::path& path)
{
    SettingsPrivate::s_path = path;
}

const Settings& Settings::instance()
{
    static Settings settings(SettingsPrivate::s_path);
    return settings;
}

Settings::Settings(const std::filesystem::path& path)
    : utils::Settings(path)
    , m_(std::make_unique<SettingsPrivate>())
{
}

} // namespace settings
} // namespace core
} // namespace simplex
