#include <utils/logger.h>

#include <core/scene.h>
#include <core/imagebasedllightnode.h>
#include <core/settings.h>
#include <core/material.h>

#include "graphicsengineprivate.h"
#include "imagebasedllightnodeprivate.h"
#include "resources.h"

namespace simplex
{
namespace core
{

ImageBasedLightNode::ImageBasedLightNode(const std::string &name)
    : LightNode(std::make_unique<ImageBasedLightNodePrivate>(*this, name))
{
    setBRDFLutMap(std::make_shared<MaterialMap>(resources::DefaultIBLBRDFLutMapPath));
    setDiffuseMap(std::make_shared<MaterialMap>(resources::DefaultIBLDiffuseMapPath));
    setSpecularMap(std::make_shared<MaterialMap>(resources::DefaultIBLSpecularMapPath));
    setContribution(settings::Settings::instance().graphics().ibl().contribution());

    shadow().setMode(ShadingMode::Disabled);
}

ImageBasedLightNode::~ImageBasedLightNode() = default;

std::shared_ptr<ImageBasedLightNode> ImageBasedLightNode::asIBLLightNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<ImageBasedLightNode>(wp.lock());
}

std::shared_ptr<const ImageBasedLightNode> ImageBasedLightNode::asIBLLightNode() const
{
    return const_cast<ImageBasedLightNode*>(this)->asIBLLightNode();
}

std::shared_ptr<MaterialMap> ImageBasedLightNode::BRDFLutMap()
{
    return m().BRDFLutMap();
}

std::shared_ptr<const MaterialMap> ImageBasedLightNode::BRDFLutMap() const
{
    return const_cast<ImageBasedLightNode*>(this)->BRDFLutMap();
}

void ImageBasedLightNode::setBRDFLutMap(const std::shared_ptr<MaterialMap>& value)
{
    auto& mPrivate = m();

    mPrivate.BRDFLutMap() = value;
    mPrivate.changeInSceneData();
}

std::shared_ptr<MaterialMap> ImageBasedLightNode::diffuseMap()
{
    return m().diffuseMap();
}

std::shared_ptr<const MaterialMap> ImageBasedLightNode::diffuseMap() const
{
    return const_cast<ImageBasedLightNode*>(this)->diffuseMap();
}

void ImageBasedLightNode::setDiffuseMap(const std::shared_ptr<MaterialMap>& value)
{
    auto& mPrivate = m();

    mPrivate.diffuseMap() = value;
    mPrivate.changeInSceneData();
}

std::shared_ptr<MaterialMap> ImageBasedLightNode::specularMap()
{
    return m().specularMap();
}

std::shared_ptr<const MaterialMap> ImageBasedLightNode::specularMap() const
{
    return const_cast<ImageBasedLightNode*>(this)->specularMap();
}

void ImageBasedLightNode::setSpecularMap(const std::shared_ptr<MaterialMap>& value)
{
    auto& mPrivate = m();

    mPrivate.specularMap() = value;
    mPrivate.changeInSceneData();
}

float ImageBasedLightNode::contribution() const
{
    return m().contribution();
}

void ImageBasedLightNode::setContribution(float value)
{
    auto& mPrivate = m();

    mPrivate.contribution() = value;
    mPrivate.changeInSceneData();
}

}
}
