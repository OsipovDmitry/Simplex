#include <utils/epsilon.h>
#include <utils/clipspace.h>
#include <utils/range.h>

#include <core/settings.h>
#include <core/shadow.h>
#include <core/ssao.h>

namespace simplex
{
namespace core
{
namespace settings
{

namespace Conversions
{

utils::ClipSpaceType string2ClipSpaceType(const std::string &value)
{
    static std::unordered_map<std::string, utils::ClipSpaceType> s_table {
        { "ortho", utils::ClipSpaceType::Ortho },
        { "perspective", utils::ClipSpaceType::Perspective }
    };

    auto it = s_table.find(value);
    return (it != s_table.end()) ? it->second : utils::ClipSpaceType::Perspective;
}

ShadingMode string2ShadingMode(const std::string &value)
{
    static std::unordered_map<std::string, ShadingMode> s_table {
        { "disabled", ShadingMode::Disabled },
        { "opaque", ShadingMode::Opaque },
        { "opaqueandtransparent", ShadingMode::OpaqueAndTransparent },
        { "color", ShadingMode::Color }
    };

    auto it = s_table.find(value);
    return (it != s_table.end()) ? it->second : ShadingMode::Disabled;
}

ShadingFilter string2ShadingFilter(const std::string &value)
{
    static std::unordered_map<std::string, ShadingFilter> s_table {
        { "point", ShadingFilter::Point },
        { "pcf", ShadingFilter::PCF },
        { "vsm", ShadingFilter::VSM },
    };

    auto it = s_table.find(value);
    return (it != s_table.end()) ? it->second : ShadingFilter::VSM;
}

SSAOMode string2SSAOMode(const std::string &value)
{
    static std::unordered_map<std::string, SSAOMode> s_table {
        { "disabled", SSAOMode::Disabled },
        { "opaque", SSAOMode::Opaque },
        { "opaqueandtransparent", SSAOMode::OpaqueAndTransparent },
        { "color", SSAOMode::Color }
    };

    auto it = s_table.find(value);
    return (it != s_table.end()) ? it->second : SSAOMode::Color;
}

}

Scene::Scene(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)
{
}

Scene::~Scene() = default;

const std::string &Scene::sceneRootNodeName() const
{
    static const auto s_name = readString("RootNodeName", "SceneRootNode");
    return s_name;
}

const std::string &Scene::listenerNodeName() const
{
    static const auto s_name = readString("ListenerNodeName", "ListenerNode");
    return s_name;
}

Application::Application(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)
{
}

Application::~Application() = default;

const std::string &Application::name() const
{
    static const auto s_name = readString("Name", "Simplex3DEngine");
    return s_name;
}

const Scene &Application::scene() const
{
    static const Scene s_scene(read("Scene"));
    return s_scene;
}

ClipSpace::ClipSpace(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)
{
}

ClipSpace::~ClipSpace() = default;

float ClipSpace::orthoHeight() const
{
    return readSingle("OrthoHeight", 1.f);
}

float ClipSpace::perspectiveFOV() const
{
    return readSingle("PerspectiveFOV", 1.570796f);
}

utils::ClipSpaceType ClipSpace::type() const
{
    auto typeString = readString("Type", "perspective");
    for (auto &c : typeString)
        c = static_cast<char>(std::tolower(c));

    return Conversions::string2ClipSpaceType(typeString);
}

Camera::Camera(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)
{
}

Camera::~Camera() = default;

const ClipSpace &Camera::clipSpace() const
{
    static const ClipSpace s_clipsapce(read("ClipSpace"));
    return s_clipsapce;
}

Background::Background(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)
{
}

Background::~Background() = default;

const glm::vec3 &Background::environmentColor() const
{
    static const auto s_color = readVec3("EnvironmentColor", glm::vec3(1.f));
    return s_color;
}

float Background::blurPower() const
{
    return readSingle("BlurPower", 1.f);
}

Flat::Flat(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)
{
}

Flat::~Flat() = default;

PBR::PBR(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)
{
}

PBR::~PBR() = default;

const glm::vec4 &Flat::color() const
{
    static const auto s_color = readVec4("Color", glm::vec4(1.f));
    return s_color;
}

float PBR::dielectricSpecular() const
{
    return readSingle("DielectricSpecular", .04f);
}

const glm::u32vec3 &PBR::ORMSwizzleMask() const
{
    static const auto s_ORMSwizzleMask = readUVec3("ORMSwizzleMask", glm::u32vec3(0u));
    return s_ORMSwizzleMask;
}

const glm::vec4 &PBR::baseColor() const
{
    static const auto s_baseColor = readVec4("BaseColor", glm::vec4(1.f));
    return s_baseColor;
}

float PBR::roughness() const
{
    return readSingle("Roughness", 1.f);
}

float PBR::metalness() const
{
    return readSingle("Metalness", 1.f);
}

float PBR::normalMapScale() const
{
    return readSingle("NormalMapScale", 1.f);
}

float PBR::occlusionMapStrength() const
{
    return readSingle("OcclusionMapStrngth", 1.f);
}

const glm::vec3 &PBR::emission() const
{
    static const auto s_emissionColor = readVec3("Emission", glm::vec3(0.f));
    return s_emissionColor;
}

IBL::IBL(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)
{
}

IBL::~IBL() = default;

float IBL::contribution() const
{
    return readSingle("Contribution", .4f);
}

Shadow::Shadow(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)
{
}

Shadow::~Shadow() = default;

ShadingMode Shadow::mode() const
{
    auto modeString = readString("Mode", "disabled");
    for (auto &c : modeString)
        c = static_cast<char>(std::tolower(c));

    return Conversions::string2ShadingMode(modeString);
}

ShadingFilter Shadow::filter() const
{
    auto filterString = readString("Filter", "vsm");
    for (auto &c : filterString)
        c = static_cast<char>(std::tolower(c));

    return Conversions::string2ShadingFilter(filterString);
}

float Shadow::depthBias() const
{
    return readSingle("DepthBias", .05f);
}

uint32_t Shadow::mapSize() const
{
    return readUint("MapSize", 512u);
}

SSAO::SSAO(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)
{
}

SSAO::~SSAO() = default;

SSAOMode SSAO::mode() const
{
    auto modeString = readString("Mode", "color");
    for (auto &c : modeString)
        c = static_cast<char>(std::tolower(c));

    return Conversions::string2SSAOMode(modeString);
}

float SSAO::viewportDenominator() const
{
    return readSingle("ViewportDenominator", 1.f);
}

float SSAO::contribution() const
{
    return readSingle("Contribution", 1.f);
}

uint32_t SSAO::kernelSize() const
{
    return readUint("KernelSize", 16u);
}

uint32_t SSAO::noiseSize() const
{
    return readUint("NoiseSize", 4u);
}

float SSAO::radius() const
{
    return readSingle("Radius", .3f);
}

Blur::Blur(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)
{
}

Blur::~Blur() = default;

float Blur::sigma() const
{
    return readSingle("Sigma", 2.f);
}

OIT::OIT(const rapidjson::Document::ValueType* value)
    : utils::SettingsComponent(value)
{
}

OIT::~OIT() = default;

uint32_t OIT::nodesPerPixel() const
{
    return readUint("NodesPerPixel", 4u);
}

uint32_t OIT::maxNodes() const
{
    return readUint("MaxNodes", 1920u * 1024u * 4u);
}

NodeBoundingBox::NodeBoundingBox(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)
{
}

NodeBoundingBox::~NodeBoundingBox() = default;

bool NodeBoundingBox::isEnabled() const
{
    return readBool("IsEnabled", false);
}

const glm::vec4 &NodeBoundingBox::color() const
{
    static const auto s_color = readVec4("Color", glm::vec4(1.f, 0.f, 0.f, 1.f));
    return s_color;
}

VisualDrawableNodeLocalBoundingBox::VisualDrawableNodeLocalBoundingBox(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)
{
}

VisualDrawableNodeLocalBoundingBox::~VisualDrawableNodeLocalBoundingBox() = default;

bool VisualDrawableNodeLocalBoundingBox::isEnabled() const
{
    return readBool("IsEnabled", false);
}

const glm::vec4 &VisualDrawableNodeLocalBoundingBox::color() const
{
    static const auto s_color = readVec4("Color", glm::vec4(0.f, 1.f, 0.f, 1.f));
    return s_color;
}

VisualDrawableBoundingBox::VisualDrawableBoundingBox(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)
{
}

VisualDrawableBoundingBox::~VisualDrawableBoundingBox() = default;

bool VisualDrawableBoundingBox::isEnabled() const
{
    return readBool("IsEnabled", false);
}

const glm::vec4 &VisualDrawableBoundingBox::color() const
{
    static const auto s_color = readVec4("Color", glm::vec4(0.f, 0.f, 1.f, 1.f));
    return s_color;
}

LightNodeAreaBoundingBox::LightNodeAreaBoundingBox(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)
{
}

LightNodeAreaBoundingBox::~LightNodeAreaBoundingBox() = default;

bool LightNodeAreaBoundingBox::isEnabled() const
{
    return readBool("IsEnabled", false);
}

const glm::vec4 &LightNodeAreaBoundingBox::color() const
{
    static const auto s_color = readVec4("Color", glm::vec4(1.f, 1.f, 1.f, .2f));
    return s_color;
}

DebugRendering::DebugRendering(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)
{
}

DebugRendering::~DebugRendering() = default;

const NodeBoundingBox &DebugRendering::nodeBoundingBox() const
{
    static const NodeBoundingBox s_nodeBoundingBox(read("NodeBoundingBox"));
    return s_nodeBoundingBox;
}

const VisualDrawableNodeLocalBoundingBox &DebugRendering::visualDrawableNodeLocalBoundingBox() const
{
    static const VisualDrawableNodeLocalBoundingBox s_visualDrawableNodeLocalBoundingBox(read("VisualDrawableNodeLocalBoundingBox"));
    return s_visualDrawableNodeLocalBoundingBox;
}

const VisualDrawableBoundingBox &DebugRendering::visualDrawableBoundingBox() const
{
    static const VisualDrawableBoundingBox s_visualDrawableBoundingBox(read("VisualDrawableBoundingBox"));
    return s_visualDrawableBoundingBox;
}

const LightNodeAreaBoundingBox &DebugRendering::lightNodeAreaBoundingBox() const
{
    static const LightNodeAreaBoundingBox s_lightNodeAreaBoundingBox(read("LightNodeAreaBoundingBox"));
    return s_lightNodeAreaBoundingBox;
}

Graphics::Graphics(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)

{
}

Graphics::~Graphics() = default;

const utils::Range &Graphics::cullPlaneLimits() const
{
    static const utils::Range s_cullPlaneLimits(readVec2("CullPlaneLimits", glm::vec2(.1f, 1000000.f)));
    return s_cullPlaneLimits;
}

const Camera &Graphics::camera() const
{
    static const Camera s_camera(read("Camera"));
    return s_camera;
}

const Background &Graphics::background() const
{
    static const Background s_background(read("Background"));
    return s_background;
}

const Flat &Graphics::flat() const
{
    static const Flat s_flat(read("Flat"));
    return s_flat;
}

const PBR &Graphics::pbr() const
{
    static const PBR s_PBR(read("PBR"));
    return s_PBR;
}

const IBL &Graphics::ibl() const
{
    static const IBL s_IBL(read("IBL"));
    return s_IBL;
}

const Shadow &Graphics::shadow() const
{
    static const Shadow s_shadow(read("Shadow"));
    return s_shadow;
}

const SSAO &Graphics::ssao() const
{
    static const SSAO s_SSAO(read("SSAO"));
    return s_SSAO;
}

const Blur &Graphics::blur() const
{
    static const Blur s_blur(read("Blur"));
    return s_blur;
}

const OIT& Graphics::oit() const
{
    static const OIT s_OIT(read("OIT"));
    return s_OIT;
}

const DebugRendering &Graphics::debugRendering() const
{
    static const DebugRendering s_debugRender(read("DebugRendering"));
    return s_debugRender;
}

Source::Source(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)
{
}

Source::~Source() = default;

float Source::gain() const
{
    return readSingle("Gain", 1.f);
}

float Source::minGain() const
{
    return readSingle("MinGain", 0.f);
}

float Source::maxGain() const
{
    return readSingle("MaxGain", 1.f);
}

float Source::referenceDistance() const
{
    return readSingle("ReferenceDistance", 1.f);
}

float Source::rolloffFactor() const
{
    return readSingle("RolloffFactor", 1.f);
}

float Source::maxDistance() const
{
    return readSingle("MaxDistance", utils::maximum<float>());
}

float Source::pitch() const
{
    return readSingle("Pitch", 1.f);
}


Listener::Listener(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)
{
}

Listener::~Listener() = default;

float Listener::gain() const
{
    return readSingle("Gain", 1.f);
}

Audio::Audio(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)
{
}

Audio::~Audio() = default;

const Source &Audio::source() const
{
    static const Source s_source(read("Source"));
    return s_source;
}

const Listener &Audio::listener() const
{
    static const Listener s_listener(read("Listener"));
    return s_listener;
}

Physics::Physics(const rapidjson::Document::ValueType *value)
    : utils::SettingsComponent(value)
{
}

Physics::~Physics() = default;

class SettingsPrivate
{
public:
    SettingsPrivate() {}
    ~SettingsPrivate() = default;

    static std::filesystem::path s_path;
};

std::filesystem::path SettingsPrivate::s_path = "./resources/settings/settings.json";

Settings::~Settings() = default;


const Application &Settings::application() const
{
    static const Application s_application(read("Application"));
    return s_application;
}

const Graphics &Settings::graphics() const
{
    static const Graphics s_graphics(read("Graphics"));
    return s_graphics;
}

const Audio &Settings::audio() const
{
    static const Audio s_audio(read("Audio"));
    return s_audio;
}

const Physics &Settings::physics() const
{
    static const Physics s_physics(read("Physics"));
    return s_physics;
}

void Settings::setPath(const std::filesystem::path &path)
{
    SettingsPrivate::s_path = path;
}

const Settings &Settings::instance()
{
    static Settings settings(SettingsPrivate::s_path);
    return settings;
}

Settings::Settings(const std::filesystem::path &path)
    : utils::Settings(path)
    , m_(std::make_unique<SettingsPrivate>())
{
}

}
}
}
