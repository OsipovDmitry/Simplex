#ifndef CORE_SETTINGS_H
#define CORE_SETTINGS_H

#include <utils/forwarddecl.h>
#include <utils/settings.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{
namespace settings
{

class CORE_SHARED_EXPORT Scene : public utils::SettingsComponent
{
public:
    Scene(const rapidjson::Document::ValueType*);
    ~Scene() override;

    const std::string &sceneRootNodeName() const;
    const std::string &listenerNodeName() const;
};

class CORE_SHARED_EXPORT Application : public utils::SettingsComponent
{
public:
    Application(const rapidjson::Document::ValueType*);
    ~Application() override;

    const std::string &name() const;
    const Scene &scene() const;
};

class CORE_SHARED_EXPORT ClipSpace : public utils::SettingsComponent
{
public:
    ClipSpace(const rapidjson::Document::ValueType*);
    ~ClipSpace() override;

    float orthoHeight() const;
    float perspectiveFOV() const;
    utils::ClipSpaceType type() const;
};

class CORE_SHARED_EXPORT Camera : public utils::SettingsComponent
{
public:
    Camera(const rapidjson::Document::ValueType*);
    ~Camera() override;

    const ClipSpace &clipSpace() const;
};

class CORE_SHARED_EXPORT Background : public utils::SettingsComponent
{
public:
    Background(const rapidjson::Document::ValueType*);
    ~Background() override;

    const glm::vec3 &color() const;
    float roughness() const;
};

class CORE_SHARED_EXPORT Flat : public utils::SettingsComponent
{
public:
    Flat(const rapidjson::Document::ValueType*);
    ~Flat() override;

    const glm::vec4 &color() const;
};

class CORE_SHARED_EXPORT PBR : public utils::SettingsComponent
{
public:
    PBR(const rapidjson::Document::ValueType*);
    ~PBR() override;

    float dielectricSpecular() const;
    const glm::u32vec3 &ORMSwizzleMask() const;
    const glm::vec4 &baseColor() const;
    float roughness() const;
    float metalness() const;
    float normalMapScale() const;
    float occlusionMapStrength() const;
    const glm::vec3 &emission() const;
};

class CORE_SHARED_EXPORT IBL : public utils::SettingsComponent
{
public:
    IBL(const rapidjson::Document::ValueType*);
    ~IBL() override;

    float contribution() const;
};

class CORE_SHARED_EXPORT Shadow : public utils::SettingsComponent
{
public:
    Shadow(const rapidjson::Document::ValueType*);
    ~Shadow() override;

    ShadingMode mode() const;
    ShadingFilter filter() const;
    float depthBias() const;
    uint32_t mapSize() const;
};

class CORE_SHARED_EXPORT SSAO : public utils::SettingsComponent
{
public:
    SSAO(const rapidjson::Document::ValueType*);
    ~SSAO() override;

    SSAOMode mode() const;
    float viewportDenominator() const;
    float contribution() const;
    uint32_t kernelSize() const;
    uint32_t noiseSize() const;
    float radius() const;
};

class CORE_SHARED_EXPORT Blur : public utils::SettingsComponent
{
public:
    Blur(const rapidjson::Document::ValueType*);
    ~Blur() override;

    float sigma() const;
};

class CORE_SHARED_EXPORT NodeBoundingBox : public utils::SettingsComponent
{
public:
    NodeBoundingBox(const rapidjson::Document::ValueType*);
    ~NodeBoundingBox() override;

    bool isEnabled() const;
    const glm::vec4 &color() const;
};

class CORE_SHARED_EXPORT VisualDrawableNodeLocalBoundingBox : public utils::SettingsComponent
{
public:
    VisualDrawableNodeLocalBoundingBox(const rapidjson::Document::ValueType*);
    ~VisualDrawableNodeLocalBoundingBox() override;

    bool isEnabled() const;
    const glm::vec4 &color() const;
};

class CORE_SHARED_EXPORT VisualDrawableBoundingBox : public utils::SettingsComponent
{
public:
    VisualDrawableBoundingBox(const rapidjson::Document::ValueType*);
    ~VisualDrawableBoundingBox() override;

    bool isEnabled() const;
    const glm::vec4 &color() const;
};

class CORE_SHARED_EXPORT LightNodeAreaBoundingBox : public utils::SettingsComponent
{
public:
    LightNodeAreaBoundingBox(const rapidjson::Document::ValueType*);
    ~LightNodeAreaBoundingBox() override;

    bool isEnabled() const;
    const glm::vec4 &color() const;
};

class CORE_SHARED_EXPORT DebugRendering : public utils::SettingsComponent
{
public:
    DebugRendering(const rapidjson::Document::ValueType*);
    ~DebugRendering() override;

    const NodeBoundingBox &nodeBoundingBox() const;
    const VisualDrawableNodeLocalBoundingBox &visualDrawableNodeLocalBoundingBox() const;
    const VisualDrawableBoundingBox &visualDrawableBoundingBox() const;
    const LightNodeAreaBoundingBox &lightNodeAreaBoundingBox() const;
};

class CORE_SHARED_EXPORT Graphics : public utils::SettingsComponent
{
public:
    Graphics(const rapidjson::Document::ValueType*);
    ~Graphics() override;

    const utils::Range &cullPlaneLimits() const;
    const Camera &camera() const;
    const Background &background() const;
    const Flat &flat() const;
    const PBR &pbr() const;
    const IBL &ibl() const;
    const Shadow &shadow() const;
    const SSAO &ssao() const;
    const Blur &blur() const;
    const DebugRendering &debugRendering() const;
    uint32_t maxFragmants() const;
};

class CORE_SHARED_EXPORT Source : public utils::SettingsComponent
{
public:
    Source(const rapidjson::Document::ValueType*);
    ~Source() override;

    float gain() const;
    float minGain() const;
    float maxGain() const;
    float referenceDistance() const;
    float rolloffFactor() const;
    float maxDistance() const;
    float pitch() const;
};

class CORE_SHARED_EXPORT Listener : public utils::SettingsComponent
{
public:
    Listener(const rapidjson::Document::ValueType*);
    ~Listener() override;

    float gain() const;
};

class CORE_SHARED_EXPORT Audio : public utils::SettingsComponent
{
public:
    Audio(const rapidjson::Document::ValueType*);
    ~Audio() override;

    const Source &source() const;
    const Listener &listener() const;
};

class CORE_SHARED_EXPORT Physics : public utils::SettingsComponent
{
public:
    Physics(const rapidjson::Document::ValueType*);
    ~Physics() override;
};

class SettingsPrivate;
class CORE_SHARED_EXPORT Settings : public utils::Settings
{
public:
    ~Settings() override;

    const Application &application() const;
    const Graphics &graphics() const;
    const Audio &audio() const;
    const Physics &physics() const;

    static void setPath(const std::filesystem::path&);
    static const Settings &instance();

protected:
    Settings(const std::filesystem::path&);

private:
    std::unique_ptr<SettingsPrivate> m_;
};

}
}
}

#endif // CORE_SETTINGS_H
