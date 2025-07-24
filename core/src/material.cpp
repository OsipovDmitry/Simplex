#include <utils/glm/vec3.hpp>
#include <utils/glm/vec4.hpp>

#include <core/material.h>

#include "materialprivate.h"

namespace simplex
{
namespace core
{

MaterialMap::MaterialMap()
    : m_(std::make_unique<MaterialMapPrivate>())
{
}

MaterialMap::MaterialMap(const std::filesystem::path& value)
    : MaterialMap()
{
    setFilesystemPath(value);
}

MaterialMap::MaterialMap(const std::shared_ptr<const utils::Image>& value)
    : MaterialMap()
{
    setImage(value);
}

MaterialMap::~MaterialMap() = default;

bool MaterialMap::isEmpty() const
{
    return asFilesystemPath().empty() && !asImage();
}

const std::filesystem::path MaterialMap::asFilesystemPath() const
{
    return m_->path();
}

std::shared_ptr<const utils::Image> MaterialMap::asImage() const
{
    return m_->image();
}

void MaterialMap::setFilesystemPath(const std::filesystem::path& value)
{
    m_->image() = nullptr;
    m_->path() = value;
    m_->onChanged();
}

void MaterialMap::setImage(const std::shared_ptr<const utils::Image>& value)
{
    m_->image() = value;
    m_->path().clear();
    m_->onChanged();
}

Material::Material()
    : m_(std::make_unique<MaterialPrivate>())
{

}
Material::~Material() = default;

bool Material::isLighted() const
{
    return m_->isLighted();
}
void Material::setIsLighted(bool value)
{
    m_->isLighted() = value;
    m_->onChanged();
}

bool Material::isShadowed() const
{
    return m_->isShadowed();
}

void Material::setIsShadowed(bool value)
{
    m_->isShadowed() = value;
    m_->onChanged();
}

bool Material::isShadowCasted() const
{
    return m_->isShadowCasted();
}

void Material::setIsShadowCasted(bool value)
{
    m_->isShadowCasted() = value;
    m_->onChanged();
}

bool Material::isDoubleSided() const
{
    return m_->isDoubleSided();
}

void Material::setDoubleSided(bool value)
{
    m_->isDoubleSided() = value;
    m_->onChanged();
}

const glm::u32vec3& Material::ORMSwizzleMask() const
{
    return m_->ORMSwizzleMask();
}

void Material::setORMSwizzleMask(const glm::u32vec3& value)
{
    m_->ORMSwizzleMask() = value;
    m_->onChanged();
}

const glm::vec4& Material::baseColor() const
{
    return m_->baseColor();
}

void Material::setBaseColor(const glm::vec4& value)
{
    m_->baseColor() = value;
    m_->onChanged();
}

const glm::vec3& Material::emission() const
{
    return m_->emission();
}

void Material::setEmission(const glm::vec3& value)
{
    m_->emission() = value;
    m_->onChanged();
}

float Material::occlusionMapStrength() const
{
    return m_->occlusionMapStrength();
}

void Material::setOcclusionMapStrength(float value)
{
    m_->occlusionMapStrength() = value;
    m_->onChanged();
}

float Material::roughness() const
{
    return m_->roughness();
}

void Material::setRoughness(float value)
{
    m_->roughness() = value;
    m_->onChanged();
}

float Material::metalness() const
{
    return m_->metalness();
}

void Material::setMetalness(float value)
{
    m_->metalness() = value;
    m_->onChanged();
}

float Material::normalMapScale() const
{
    return m_->normalMapScale();
}

void Material::setNormalMapScale(float value)
{
    m_->normalMapScale() = value;
    m_->onChanged();
}

std::shared_ptr<const MaterialMap> Material::materialMap(MaterialMapTarget value) const
{
    return m_->maps()[castFromMaterialMapTarget(value)];
}

void Material::setMaterialMap(MaterialMapTarget target, std::shared_ptr<const MaterialMap>& map)
{
    m_->maps()[castFromMaterialMapTarget(target)] = map;
    m_->onChanged();
}

}
}
