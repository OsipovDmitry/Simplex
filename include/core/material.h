#ifndef CORE_MATERIAL_H
#define CORE_MATERIAL_H

#include <memory>
#include <filesystem>

#include <utils/noncopyble.h>
#include <utils/pimpl.h>
#include <utils/enumclass.h>
#include <utils/forwarddecl.h>
#include <utils/image.h>
#include <utils/glm/vec3.hpp>
#include <utils/glm/vec4.hpp>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class MaterialMapPrivate;
class CORE_SHARED_EXPORT MaterialMap
{
    NONCOPYBLE(MaterialMap)
    PRIVATE_IMPL(MaterialMap)
public:
    MaterialMap();
    MaterialMap(const std::filesystem::path&);
    MaterialMap(const std::shared_ptr<utils::Image>&);
    ~MaterialMap();

    bool isEmpty() const;

    const std::filesystem::path& filesystemPath() const;
    std::shared_ptr<const utils::Image> image() const;

    void setFilesystemPath(const std::filesystem::path&);
    void setImage(const std::shared_ptr<utils::Image>&);

private:
    std::unique_ptr<MaterialMapPrivate> m_;

};

ENUMCLASS(MaterialMapTarget, uint16_t,
    BaseColor,
    Opacity,
    Emission,
    Occlusion,
    Roughness,
    Metalness,
    Normal);

class MaterialPrivate;
class CORE_SHARED_EXPORT Material
{
    NONCOPYBLE(Material)
    PRIVATE_IMPL(Material)

public:
    Material();
    ~Material();

    bool isLighted() const;
    void setIsLighted(bool);

    bool isShadowed() const;
    void setIsShadowed(bool);

    bool isShadowCasted() const;
    void setIsShadowCasted(bool);

    bool isDoubleSided() const;
    void setDoubleSided(bool);

    const glm::u32vec3& ORMSwizzleMask() const;
    void setORMSwizzleMask(const glm::u32vec3&);

    const glm::vec4& baseColor() const;
    void setBaseColor(const glm::vec4&);

    const glm::vec3& emission() const;
    void setEmission(const glm::vec3&);

    float occlusionMapStrength() const;
    void setOcclusionMapStrength(float);

    float roughness() const;
    void setRoughness(float);

    float metalness() const;
    void setMetalness(float);

    float normalMapScale() const;
    void setNormalMapScale(float);

    float alphaCutoff() const;
    void setAlphaCutoff(float);

    std::shared_ptr<const MaterialMap> materialMap(MaterialMapTarget) const;
    void setMaterialMap(MaterialMapTarget, const std::shared_ptr<MaterialMap>&);

private:
    std::unique_ptr<MaterialPrivate> m_;
};

}
}

#endif // CORE_MATERIAL_H
