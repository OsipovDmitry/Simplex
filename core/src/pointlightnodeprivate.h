#ifndef CORE_POINTLIGHTNODEPRIVATE_H
#define CORE_POINTLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class PointLightNodePrivate : public LightNodePrivate
{
public:
    PointLightNodePrivate(const std::string &name);
    ~PointLightNodePrivate() override;

    static std::weak_ptr<graphics::IVertexArray> &lightAreaVertexArray();
    static utils::BoundingBox &lightAreaBoundingBox();

private:
    static std::weak_ptr<graphics::IVertexArray> s_lightAreaVertexArray;
    static utils::BoundingBox s_lightAreaBoundingBox;

};

}
}

#endif // CORE_POINTLIGHTNODEPRIVATE_H
